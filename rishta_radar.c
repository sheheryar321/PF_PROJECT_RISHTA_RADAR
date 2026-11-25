#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NQ 8
#define NT 3
#define NC 5
#define MAXM 5

const char *typeNames[NT] = {"GHAR KA CHIRAAGH", "CAREER KI UDAAN", "MEHFIL KA SITARA"};
const char *typeAdvice[NT] = {
    "You value home and family warmth. Look for someone who respects family, not just 'joint vs nuclear'.",
    "You value growth and purpose. Look for a partner who supports your ambitions instead of feeling threatened.",
    "You value people and energy. Look for someone comfortable with your friends, your circle and new places."};
const char *critNames[NC] = {"Family Respect", "Honest Communication", "Career Freedom",
     "Shared Lifestyle Values", "Emotional Maturity"};

const char *questions[NQ] = {
    "Where would you like to live after marriage?",
    "What is your outlook on career?",
    "Your ideal weekend?",
    "After an argument, you usually...",
    "Most important thing in a rishta?",
    "How do you spend Eid?",
    "Where do you see yourself in 5 years?",
    "You get a surprise bonus. You..."};
const char *options[NQ][3] = {
    {"Joint family, everyone together", "Separate home, weekly visits", "Fully independent"},
    {"Climb high, promotion after promotion", "Balance work and home", "Home and family come first"},
    {"Big gathering with friends", "Small close circle", "Alone with a book or show"},
    {"Talk it out right away", "Take space, then talk", "Avoid it and hope it passes"},
    {"Family environment", "Matching ambitions", "Friendly, open nature"},
    {"With the whole khandan", "With friends", "Travel or quiet time"},
    {"Settled home and family", "Big career or business", "Lots of travel and experiences"},
    {"Spend it on parents or family", "Invest in a course or business", "Treat friends, plan a trip"}};

int weights[NQ][3][NT] = {
    {{3,0,0},{2,0,1},{0,1,1}}, {{0,3,0},{1,2,0},{2,0,0}},
    {{0,0,3},{0,0,2},{0,1,0}}, {{1,0,1},{1,1,0},{0,0,0}},
    {{3,0,0},{0,3,0},{0,0,3}}, {{3,0,1},{0,0,3},{0,1,1}},
    {{3,0,0},{0,3,0},{0,1,2}}, {{3,0,0},{0,3,0},{0,0,3}}};

const char *aunty[5] = {
    "Appa: 'Wah beta, soch samajh ke jawab de rahe ho.'",
    "Appa: 'Hmm... yeh to dilchasp hai.'",
    "Appa: 'Mere zamane mein yeh sawal nahi poochte thay!'",
    "Appa: 'Chai pee lo, phir agla sawal.'",
    "Appa: 'Mashallah, honest jawab hai.'"};

const char *prepQ[NT][5] = {
    {"How would you describe your relationship with your parents?",
     "What does a normal family weekend look like for you?",
     "How do you handle disagreements with elders?",
     "What does 'respecting each other's families' mean to you?",
     "What traditions matter most to you?"},
    {"What are you working toward in the next 5 years?",
     "How would you feel if I had a very demanding career phase?",
     "How do you picture sharing household responsibilities?",
     "What was the last big goal you achieved?",
     "How do you handle stress at work?"},
    {"What do you do to unwind with friends?",
     "How comfortable are you meeting new people?",
     "What is your idea of a perfect trip?",
     "How do you feel about hosting guests at home?",
     "How do you balance friends and family time?"}};

const char *noMsgs[4][2] = {
    {"Thank you so much for your time and kindness. I feel I am not ready yet, but I truly wish you all the best.",
     "We appreciate the opportunity. At this time we are not ready to proceed. We wish your family well."},
    {"It was lovely meeting you. After thinking about it, I feel our values and expectations differ, and it is best to be honest now.",
     "Thank you for meeting us. After consideration, we feel our family values do not align. We wish you well."},
    {"I am grateful for the proposal. I want to focus on my career right now, and I hope you understand.",
     "Thank you for considering us. At present the focus is on career and studies. We respectfully decline."},
    {"You are a good person, but I did not feel a connection. I would rather be honest than keep you waiting.",
     "Thank you for your time. We did not feel a connection and do not wish to waste yours. Best wishes."}};
const char *pressure[3][3] = {
    {"Aunty, jab hoga tab sab se pehle aap ko hi bataunga/bataungi! Tab tak dua kijiye.",
     "Aunty, main khud tayyar hoon jab sahi insaan milega. Mujhe thoda waqt dijiye.",
     "Aunty, aap ki fikar ka shukriya. Main sahi waqt par khud family se baat karunga/karungi."},
    {"Bhai, dua karo, rishta tum hi dhoond do... bas meri list zara lambi hai!",
     "Bhai, main jaldbazi mein faisla nahi karna chahta/chahti. Waqt aane par pata chal jayega.",
     "Thank you for caring. I am working on myself first and will share news when there is any."},
    {"Aap ke ghar ki chai ka chakkar lagta rahe, baqi shaadi apne waqt par!",
     "Shukriya, lekin yeh mera zaati faisla hai aur main isay apne tareeqe se karunga/karungi.",
     "Jazakallah for the concern. I will let the family know when the time is right."}};
const char *myths[6][2] = {
    {"Height and complexion decide compatibility.", "Values, communication and respect decide it. Looks fade, habits stay."},
    {"A big salary means a happy marriage.", "Financial responsibility matters more than the number on the payslip."},
    {"Joint family always means more problems.", "It depends on respect and boundaries, not on the house layout."},
    {"Love will just happen after marriage.", "Effort, honesty and shared goals build it; do not leave it to chance."},
    {"Saying no to a rishta insults the family.", "A polite, honest no is far kinder than a forced yes."},
    {"Meeting twice is enough to know someone.", "Ask real questions about stress, family and goals before deciding."}};

int readInt(const char *prompt, int lo, int hi)
{
    char buf[64];
    int v;
    while (1)
    {
        printf("%s", prompt);
        if (fgets(buf, sizeof buf, stdin) == NULL) return lo;
        if (sscanf(buf, "%d", &v) == 1 && v >= lo && v <= hi) return v;
        printf("  Please enter a number from %d to %d.\n", lo, hi);
    }
}
void readLine(const char *prompt, char *buf, int size)
{
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\n")] = '\0';
}
int readYN(const char *prompt)
{
    char buf[16];
    readLine(prompt, buf, sizeof buf);
    return (buf[0] == 'y' || buf[0] == 'Y');
}
int indexOfMax(int *arr, int n)
{
    int i, best = 0;
    for (i = 1; i < n; i++)
        if (arr[i] > arr[best]) best = i;
    return best;
}

void runQuiz(int *scores, int *type)
{
    int q, t, c;
    for (t = 0; t < NT; t++) scores[t] = 0;
    for (q = 0; q < NQ; q++)
    {
        printf("\nQ%d. %s\n", q + 1, questions[q]);
        for (c = 0; c < 3; c++) printf("  %d. %s\n", c + 1, options[q][c]);
        c = readInt("Your answer: ", 1, 3) - 1;
        for (t = 0; t < NT; t++) scores[t] += weights[q][c][t];
        printf("%s\n", aunty[rand() % 5]);
    }
    *type = indexOfMax(scores, NT) + 1;
    printf("\n--- RESULT ---\nFamily: %d  Ambition: %d  Social: %d\n", scores[0], scores[1], scores[2]);
    printf("Your type: %s\n%s\n", typeNames[*type - 1], typeAdvice[*type - 1]);
}

void setNonNeg(int *prio, int *db)
{
    int i;
    printf("\nRate how important each value is to you (1 = flexible, 5 = essential).\n");
    for (i = 0; i < NC; i++)
    {
        printf("\n%s\n", critNames[i]);
        prio[i] = readInt("  Importance (1-5): ", 1, 5);
        db[i] = readYN("  Is this a deal-breaker? (y/n): ");
    }
    printf("\nSaved. Top priority: %s\n", critNames[indexOfMax(prio, NC)]);
}

void meetingPrep(int type)
{
    int i;
    if (type == 0) type = readInt("Take the quiz first, or pick a type now (1 Family, 2 Ambition, 3 Social): ", 1, 3);
    printf("\nQuestions to ask instead of 'what is your salary?' (%s):\n", typeNames[type - 1]);
    for (i = 0; i < 5; i++) printf("  %d. %s\n", i + 1, prepQ[type - 1][i]);
}

int weightedPct(int *ratings, int *prio)
{
    int i, sum = 0, max = 0;
    for (i = 0; i < NC; i++) { sum += ratings[i] * prio[i]; max += 5 * prio[i]; }
    return sum * 100 / max;
}
int hasDealbreaker(int *ratings, int *db)
{
    int i;
    for (i = 0; i < NC; i++)
        if (db[i] && ratings[i] <= 2) return 1;
    return 0;
}
void addMeeting(char names[][30], int ratings[][NC], int *count)
{
    int i;
    if (*count >= MAXM) { printf("Tracker is full (%d meetings).\n", MAXM); return; }
    readLine("Name or nickname of the person: ", names[*count], 30);
    printf("Rate each from 1 (poor) to 5 (great). Values only, no looks or salary.\n");
    for (i = 0; i < NC; i++)
    {
        printf("%s", critNames[i]);
        ratings[*count][i] = readInt(": ", 1, 5);
    }
    (*count)++;
    printf("Meeting saved.\n");
}
void rankMeetings(int ratings[][NC], int count, int *prio, int *order, int *pct)
{
    int i, j, tmp;
    for (i = 0; i < count; i++) { order[i] = i; pct[i] = weightedPct(ratings[i], prio); }
    for (i = 0; i < count - 1; i++)
        for (j = 0; j < count - 1 - i; j++)
            if (pct[order[j]] < pct[order[j + 1]]) { tmp = order[j]; order[j] = order[j + 1]; order[j + 1] = tmp; }
}
void viewMeetings(char names[][30], int ratings[][NC], int count, int *prio, int *db)
{
    int order[MAXM], pct[MAXM], i;
    if (count == 0) { printf("No meetings tracked yet.\n"); return; }
    rankMeetings(ratings, count, prio, order, pct);
    printf("\nRanked by YOUR priorities:\n");
    for (i = 0; i < count; i++)
        printf("  #%d %-20s %3d%% match %s\n", i + 1, names[order[i]], pct[order[i]],
               hasDealbreaker(ratings[order[i]], db) ? "[DEAL-BREAKER FLAG]" : "");
}

void familyGap(int *prio)
{
    int i, parent, gap;
    printf("\nYour ratings come from My Non-Negotiables. Now enter what your parents would say.\n");
    for (i = 0; i < NC; i++)
    {
        printf("%s (you: %d)", critNames[i], prio[i]);
        parent = readInt(" - parents' importance (1-5): ", 1, 5);
        gap = abs(prio[i] - parent);
        if (gap <= 1) printf("  Aligned. Good news.\n");
        else if (gap == 2) printf("  Some difference. Talk it through calmly.\n");
        else printf("  Big gap. Sit down together, maybe with a trusted elder.\n");
    }
}

void biodata(int type, int *prio)
{
    char name[40], job[40], city[40], i1[40], i2[40], i3[40];
    int age;
    readLine("Name: ", name, 40);
    age = readInt("Age: ", 18, 80);
    readLine("Profession: ", job, 40);
    readLine("City: ", city, 40);
    readLine("Interest 1: ", i1, 40);
    readLine("Interest 2: ", i2, 40);
    readLine("Interest 3: ", i3, 40);
    printf("\n==============================\n BIODATA: %s\n==============================\n", name);
    printf(" Age: %d | %s | %s\n Interests: %s, %s, %s\n", age, job, city, i1, i2, i3);
    if (type) printf(" Personality: %s\n", typeNames[type - 1]);
    printf(" What I value most: %s\n==============================\n", critNames[indexOfMax(prio, NC)]);
}

void politeNo(void)
{
    int r, t;
    printf("1 Not ready  2 Values differ  3 Focus on career  4 No connection\n");
    r = readInt("Reason: ", 1, 4);
    t = readInt("Tone (1 Warm, 2 Formal): ", 1, 2);
    printf("\n%s\n", noMsgs[r - 1][t - 1]);
}
void pressureHandler(void)
{
    int w, t;
    w = readInt("Who is asking? (1 Aunty, 2 Cousin, 3 Neighbour): ", 1, 3);
    t = readInt("Tone (1 Funny, 2 Firm, 3 Polite): ", 1, 3);
    printf("\n%s\n", pressure[w - 1][t - 1]);
}

void mythBuster(void)
{
    int m;
    do
    {
        m = rand() % 6;
        printf("\nMYTH: %s\nREALITY: %s\n", myths[m][0], myths[m][1]);
    } while (readYN("Another one? (y/n): "));
}

void guides(void)
{
    printf("\nVERIFICATION CHECKLIST (respectfully confirm before getting serious):\n");
    printf("  [ ] Job or education is what they said\n  [ ] Family background and who lives at home\n");
    printf("  [ ] Their intentions and timeline\n  [ ] Expectations about work after marriage\n");
    printf("  [ ] Ask a trusted person who actually knows them\n");
    printf("\nFIRST MEETING ETIQUETTE:\n");
    printf("  - Be on time, dress comfortably, be yourself\n  - Ask open questions, listen more than you talk\n");
    printf("  - Do not interrogate about money or looks\n  - Thank the family either way\n");
}

void burnout(void)
{
    const char *bq[4] = {"I feel tired of rishta meetings", "I feel pressured by relatives",
                         "I am saying yes just to end the pressure", "I have little time left for myself"};
    int i, total = 0;
    printf("\nRate 1 (not at all) to 5 (very much):\n");
    for (i = 0; i < 4; i++)
    {
        printf("%s", bq[i]);
        total += readInt(": ", 1, 5);
    }
    printf("\nBurnout score: %d / 20\n", total);
    if (total <= 8) printf("You are doing fine. Keep your pace.\n");
    else if (total <= 14) printf("Getting heavy. Take a short break and protect your time.\n");
    else printf("High burnout. Pause for a month. Your wellbeing matters more than any timeline.\n");
}

void finalDecision(int type, int *prio, int *db, char names[][30], int ratings[][NC], int count)
{
    int order[MAXM], pct[MAXM];
    if (count == 0) { printf("Add at least one meeting to the tracker first.\n"); return; }
    rankMeetings(ratings, count, prio, order, pct);
    printf("\n=== DECISION SUMMARY ===\n");
    if (type) printf("Your type: %s\n", typeNames[type - 1]);
    printf("Your top priority: %s\n", critNames[indexOfMax(prio, NC)]);
    printf("Best match so far: %s (%d%%)\n", names[order[0]], pct[order[0]]);
    if (hasDealbreaker(ratings[order[0]], db)) printf("Warning: this person trips one of your deal-breakers.\n");
    else printf("No deal-breakers triggered.\n");
    printf("This is a guide, not a verdict. The decision is yours and your family's.\n");
}

int main(void)
{
    int scores[NT] = {0}, type = 0, choice;
    int prio[NC] = {3, 3, 3, 3, 3}, db[NC] = {0};
    char names[MAXM][30];
    int ratings[MAXM][NC], count = 0;

    srand((unsigned)time(NULL));
    printf("=====================================\n   RISHTA RADAR - ALL IN ONE\n   Values first, biodata later.\n=====================================\n");
    do
    {
        printf("\n 1 Self-Discovery Quiz       8 Polite No Generator\n");
        printf(" 2 My Non-Negotiables        9 Pressure Handler\n");
        printf(" 3 Meeting Prep Questions   10 Appa's Myth Buster\n");
        printf(" 4 Add Meeting to Tracker   11 Checklist and Etiquette\n");
        printf(" 5 View Ranked Meetings     12 Burnout Meter\n");
        printf(" 6 Family Expectation Gap   13 Final Decision Helper\n");
        printf(" 7 Smart Biodata Maker       0 Exit\n");
        choice = readInt("Choose: ", 0, 13);
        switch (choice)
        {
            case 1: runQuiz(scores, &type); break;
            case 2: setNonNeg(prio, db); break;
            case 3: meetingPrep(type); break;
            case 4: addMeeting(names, ratings, &count); break;
            case 5: viewMeetings(names, ratings, count, prio, db); break;
            case 6: familyGap(prio); break;
            case 7: biodata(type, prio); break;
            case 8: politeNo(); break;
            case 9: pressureHandler(); break;
            case 10: mythBuster(); break;
            case 11: guides(); break;
            case 12: burnout(); break;
            case 13: finalDecision(type, prio, db, names, ratings, count); break;
            default: printf("\nKhuda hafiz! May you find the right match, not just a quick one.\n");
        }
    } while (choice != 0);
    return 0;
}