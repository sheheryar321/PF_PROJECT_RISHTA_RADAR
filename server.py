import html
import os
import re
import secrets
import shutil
import subprocess
import sys
from http import cookies
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs

HERE = os.path.dirname(os.path.abspath(__file__))
C_FILE = os.path.join(HERE, "rishta_radar.c")
EXE = os.path.join(HERE, "rishta_radar" + (".exe" if os.name == "nt" else ""))
HTML_FILE = os.path.join(HERE, "index.html")
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 8000

NC, MAXM = 5, 5
MENU_START = "\n 1 Self-Discovery Quiz"
CRIT = ["Family Respect", "Honest Communication", "Career Freedom",
        "Shared Lifestyle Values", "Emotional Maturity"]

SESSIONS = {}

def compile_c():
    if os.path.exists(EXE) and os.path.getmtime(EXE) >= os.path.getmtime(C_FILE):
        return
    cc = shutil.which("gcc") or shutil.which("cc") or shutil.which("clang")
    if not cc:
        sys.exit("No C compiler found. Install gcc (or clang) and run again.")
    print("Compiling", os.path.basename(C_FILE), "...")
    r = subprocess.run([cc, C_FILE, "-o", EXE], capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit("Compile failed:\n" + r.stderr)

def run_c(inputs):
    data = "\n".join(inputs + ["0"]) + "\n"
    p = subprocess.run([EXE], input=data, capture_output=True, text=True,
                       encoding="utf-8", errors="replace", timeout=10)
    parts = p.stdout.replace("\r\n", "\n").split("Choose: ")
    return parts[-2].split(MENU_START)[0]

def num(form, key, lo, hi, default=None):
    try:
        v = int(form.get(key, [""])[0])
    except ValueError:
        v = lo if default is None else default
    return str(max(lo, min(hi, v)))

def text(form, key, size):
    s = form.get(key, [""])[0].replace("\r", " ").replace("\n", " ").strip()
    return s[: size - 2]

def new_session():
    return {"quiz": None, "nonneg": None, "meetings": [], "results": {}}

def replay_inputs(s):
    lines = []
    if s["quiz"]:
        lines += ["1"] + s["quiz"]
    if s["nonneg"]:
        lines += ["2"] + s["nonneg"]
    for m in s["meetings"]:
        lines += ["4"] + m
    return lines

def do_action(action, form, s):
    base = replay_inputs(s)

    if action == "quiz":
        ans = [num(form, f"q{i}", 1, 3) for i in range(8)]
        out = run_c(base + ["1"] + ans)
        s["quiz"] = ans
        res = out[out.index("--- RESULT ---"):].strip()
        appa = "\n".join(re.findall(r"Appa: .*", out))
        return res + "\n\nAppa's comments:\n" + appa

    if action == "nonneg":
        ins = []
        for i in range(NC):
            ins += [num(form, f"p{i}", 1, 5), "y" if form.get(f"d{i}") else "n"]
        out = run_c(base + ["2"] + ins)
        s["nonneg"] = ins
        return re.search(r"Saved\..*", out).group(0)

    if action == "prep":
        extra = [] if s["quiz"] else [num(form, "ptype", 1, 3)]
        out = run_c(base + ["3"] + extra)
        return out[out.index("Questions to ask"):].strip()

    if action == "add":
        if len(s["meetings"]) >= MAXM:
            out = run_c(base + ["4"])
            return out.strip()
        m = [text(form, "name", 30)] + [num(form, f"r{i}", 1, 5) for i in range(NC)]
        out = run_c(base + ["4"] + m)
        s["meetings"].append(m)
        return re.search(r"Meeting saved\.", out).group(0) + f"  ({len(s['meetings'])}/{MAXM} tracked)"

    if action == "view":
        return run_c(base + ["5"]).strip()

    if action == "gap":
        parents = [num(form, f"g{i}", 1, 5) for i in range(NC)]
        out = run_c(base + ["6"] + parents)
        rows = re.findall(r"\(you: (\d+)\).*?\(1-5\):\s*(.*)", out)
        return "\n".join(f"{CRIT[i]} (you: {y}, parents: {parents[i]})\n   {msg}"
                         for i, (y, msg) in enumerate(rows))

    if action == "bio":
        ins = [text(form, "name", 40), num(form, "age", 18, 80, 25), text(form, "job", 40),
               text(form, "city", 40), text(form, "i1", 40), text(form, "i2", 40), text(form, "i3", 40)]
        out = run_c(base + ["7"] + ins)
        return re.search(r"={10,}\n BIODATA.*", out, re.S).group(0).strip()

    if action == "no":
        out = run_c(base + ["8", num(form, "reason", 1, 4), num(form, "tone", 1, 2)])
        return out.strip().splitlines()[-1]

    if action == "pressure":
        out = run_c(base + ["9", num(form, "who", 1, 3), num(form, "tone", 1, 3)])
        return out.strip().splitlines()[-1]

    if action == "myth":
        out = run_c(base + ["10", "n"])
        return re.search(r"MYTH:.*?\nREALITY:.*?(?=\n|$)", out, re.S).group(0)

    if action == "guide":
        return run_c(base + ["11"]).strip()

    if action == "burnout":
        out = run_c(base + ["12"] + [num(form, f"b{i}", 1, 5) for i in range(4)])
        return out[out.index("Burnout score"):].strip()

    if action == "final":
        out = run_c(base + ["13"])
        return re.search(r"(=== DECISION SUMMARY.*|Add at least one.*)", out, re.S).group(0).strip()

    raise KeyError(action)

def render(s):
    page = open(HTML_FILE, encoding="utf-8").read()

    def out_box(m):
        r = s["results"].get(m.group(1))
        return f'<pre class="out">{html.escape(r)}</pre>' if r else ""

    page = re.sub(r"<!--OUT:(\w+)-->", out_box, page)

    if s["quiz"]:
        prep = '<p class="stat">Using your quiz result.</p>'
    else:
        prep = ('<label>Pick a type (or take the quiz first)<select name="ptype">'
                '<option value="1">1 Family</option><option value="2">2 Ambition</option>'
                '<option value="3">3 Social</option></select></label>')
    page = page.replace("<!--PREPTYPE-->", prep)

    state = (f'<p class="stat">Quiz taken: {"yes" if s["quiz"] else "no"} &middot; '
             f'Non-negotiables saved: {"yes" if s["nonneg"] else "no (defaults)"} &middot; '
             f'Meetings tracked: {len(s["meetings"])}/{MAXM}</p>')
    return page.replace("<!--STATE-->", state)

class Handler(BaseHTTPRequestHandler):
    def session(self):
        c = cookies.SimpleCookie(self.headers.get("Cookie", ""))
        sid = c["sid"].value if "sid" in c else None
        if sid not in SESSIONS:
            sid = secrets.token_hex(16)
            SESSIONS[sid] = new_session()
        return sid, SESSIONS[sid]

    def reply(self, code, body=b"", headers=()):
        self.send_response(code)
        for k, v in headers:
            self.send_header(k, v)
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def redirect(self, sid, where):
        self.reply(303, headers=[("Location", where),
                                 ("Set-Cookie", f"sid={sid}; Path=/; HttpOnly; SameSite=Lax")])

    def do_GET(self):
        if self.path.split("?")[0] != "/":
            return self.reply(404, b"Not found", [("Content-Type", "text/plain")])
        sid, s = self.session()
        self.reply(200, render(s).encode("utf-8"),
                   [("Content-Type", "text/html; charset=utf-8"),
                    ("Set-Cookie", f"sid={sid}; Path=/; HttpOnly; SameSite=Lax")])

    def do_POST(self):
        sid, s = self.session()
        length = int(self.headers.get("Content-Length", 0))
        form = parse_qs(self.rfile.read(length).decode("utf-8", errors="replace"))
        if self.path == "/reset":
            SESSIONS[sid] = new_session()
            return self.redirect(sid, "/")
        m = re.fullmatch(r"/run/(\w+)", self.path)
        if not m:
            return self.reply(404, b"Not found", [("Content-Type", "text/plain")])
        action = m.group(1)
        try:
            s["results"][action] = do_action(action, form, s)
        except Exception as e:
            s["results"][action] = f"Something went wrong: {e}"
        self.redirect(sid, f"/#{action}")

    def log_message(self, fmt, *args):
        pass

if __name__ == "__main__":
    compile_c()
    print(f"Rishta Radar running at http://127.0.0.1:{PORT}  (Ctrl+C to stop)")
    ThreadingHTTPServer(("127.0.0.1", PORT), Handler).serve_forever()