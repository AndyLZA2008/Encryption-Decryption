#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <math.h>
#include <windows.h>
#include <graphics.h>
#include <ege/sys_edit.h>
#include <direct.h>

using namespace std;

struct buttonStates {
    bool enable;
    bool visible;
    bool pressed;
    bool checked;
    bool hovered;
};

struct circleButton {
    int x, y;
    int radius;
    color_t normalColor;
    color_t pressedColor;
    const char *text;
    buttonStates state;
};

sys_edit plainBox;
sys_edit chiperBox;
sys_edit keyBox;
sys_edit effBox;
sys_edit typefaceBox;

#define BUTTON_SIZE    8
#define BUTTON_MIN_ID  1
#define ENC            true
#define DEC            false
#define MAX_LEN        2147483640
#define KEY_DOWN(VK) ((GetAsyncKeyState(VK)& 0x8000 ? 1:0))
#define KEY_DOWN_FOREGROUND(hWnd, vk) (KEY_DOWN(vk) && GetForegroundWindow() == hWnd)


char moveString[64];
char table[256];

circleButton buttonArray[BUTTON_SIZE];
int pressButtonId = 0;
int hoveredButtonId = 0;

bool willStart = false;
bool willClear = false;
bool willCopy = false;
bool clickStart = false;
bool clickClear = false;
bool clickCopy = false;
bool stayStart = false;
bool stayClear = false;
bool stayCopy = false;
bool leaveStart = false;

string newKey;
string newSEff;
const char *newTypeface;
int iEff;

const char *oldTypeface;
string oldSEff;
string oldKey;

void changeClipboard(char *c);

color_t interpColor(color_t colorA, color_t colorB, float t);

bool insideButton(const circleButton *button, int x, int y);

circleButton *findButton(unsigned int id);

void drawButton(const circleButton *button, int id);

void drawAllButtons();

int searchButton(int x, int y);

void onlyDrawButton(const circleButton *button, color_t color);

void draw(PIMAGE pImage);

void initButtonState(circleButton *button);

void initButton(circleButton *button, int radius, int id);

void initAllButtons();

void moveStringInit();

string moveStringEncode(string moveStringStr);

string moveStringDecode(string moveStringStr);

string moveStringMain(bool opt, string moveStringStr);

void confusion(string *pEnString); // Insert false information

void ten2Two(int x, vector<char> *basicVec); // DEC -> BIN

bool getIsNegative(const string &basicString);

int distinction(string basicString); // Distinguish the true from the false

void initCode(char *capitalEncoder, char *capitalDecoder, char *lowEncoder,
              char *lowDecoder, int sK, string &s1, int bK);

string
edCode(bool op, string &st, const char *capitalEncoder, const char *capitalDecoder,
       const char *lowEncoder, const char *lowDecoder);

string keyCoder(bool op, string basicString);

void encrypt(string basicString);

void decrypt(string chiperString);

void initWindow();

void ed(string &plainString, string &chiperString);

void mouse(int &xMouse, int &yMouse, bool &mouseMoved);

bool isWrongEff();

void
setString(const int buffSize, char *strBuffer, char *strBuffer1,
          char *strBuffer2, char *strBuffer3, char *strBuffer4, string &plainString, string &chiperString);

void changeHistory(ofstream &file_writer);

void operations(string &s, string &s1, ofstream &file_writer);

void running(int buffSize, char *strBuffer, char *strBuffer1, char *strBuffer2, char *strBuffer3, char *strBuffer4,
             int xMouse, int yMouse, ofstream &file_writer);

int main() {
    srand(time(0));
    const int buffSize = 0x3f3f;
    char strBuffer[0x3f3f] = "";
    char strBuffer1[0x3f3f] = "";
    char strBuffer2[0x3f3f] = "";
    char strBuffer3[0x3f3f] = "";
    char strBuffer4[0x3f3f] = "";
    int xMouse = 0, yMouse = 0;
    string folderPath = getenv("USERPROFILE");
    for (int i = 0; i < folderPath.size(); ++i) {
        if ((int) folderPath[i] == 92) {
            folderPath[i] = '/';
        }
    }
    folderPath = folderPath + "/" + ".ed";
    if (0 != _mkdir(folderPath.c_str())) {
        _mkdir(folderPath.c_str());
    }
    string file_name = folderPath + "/Personalized_History";
    ifstream file_reader(file_name.c_str());
    string a[3] = {"IAmAdmin", "IAmAdmin", "IAmAdmin"};
    string tmp;
    int txtFlag = 0;
    while (file_reader >> tmp) {
        a[txtFlag % 3] = tmp;
        txtFlag++;
    }
    for (int i = 0; i < 3; ++i) {
        if (a[i] == "IAmAdmin") {
            a[i] = "";
        }
    }
    char tempC[0x3f3f];
    strcpy(tempC, a[0].c_str());
    oldTypeface = tempC;
    oldSEff = a[1];
    oldKey = a[2];
    newTypeface = tempC;
    newSEff = a[1];
    newKey = a[2];
    file_reader.close();
    ofstream file_writer(file_name.c_str(), ios_base::out);
    initWindow();
    moveStringInit();
    running(buffSize, strBuffer, strBuffer1,
            strBuffer2, strBuffer3, strBuffer4, xMouse, yMouse, file_writer);
    file_writer.close();
    changeHistory(file_writer);
    closegraph();
    return 0;
}

void changeClipboard(char *c) {
    HWND hwndClipboard = 0;
    OpenClipboard(hwndClipboard);
    EmptyClipboard();
    HANDLE hHandle = GlobalAlloc(GMEM_FIXED, 10000000);
    char *pData = (char *) GlobalLock(hHandle);
    strcpy(pData, c);
    SetClipboardData(CF_TEXT, hHandle);
    CloseClipboard();
    return;
}

color_t interpColor(color_t colorA, color_t colorB, float t) {
    if (t > 1) {
        t = 1;
    } else if (t < 0) {
        t = 0;
    }
    int r = roundf((1 - t) * EGEGET_R(colorA) + t * EGEGET_R(colorB));
    int g = roundf((1 - t) * EGEGET_G(colorA) + t * EGEGET_G(colorB));
    int b = roundf((1 - t) * EGEGET_B(colorA) + t * EGEGET_B(colorB));
    return EGERGB(r, g, b);
}

bool insideButton(const circleButton *button, int x, int y) {
    int dx = x - button->x, dy = y - button->y;
    return (dx * dx + dy * dy) <= (button->radius * button->radius);
}

circleButton *findButton(unsigned int id) {
    return (id == 0) ? NULL : &buttonArray[id - 1];
}

void onlyDrawButton(const circleButton *button, color_t color) {
    setfillcolor(color);
    ege_fillellipse(button->x - button->radius, button->y - button->radius,
                    2 * button->radius, 2 * button->radius);
    setcolor(interpColor(color, BLACK, 0.25f));
    setlinewidth(4);
    int borderRadius = button->radius + 4;
    ege_ellipse(button->x - borderRadius, button->y - borderRadius,
                2 * borderRadius, 2 * borderRadius);
    setcolor(WHITE);
    xyprintf(button->x, button->y, "%s", button->text);
    return;
}

void drawButton(const circleButton *button, int id) {
    if (button != 0) {
        if (button->state.visible) {
            setfillcolor(EGEARGB(0xFF, 0x1E, 0x90, 0xFF));
            setcolor(WHITE);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
            setfont(24, 0, newTypeface);
            color_t color;
            if (id == 1) {
                if (button->state.pressed || (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_SHIFT)
                                              && KEY_DOWN_FOREGROUND(getHWnd(), 'X'))) {
                    color = button->pressedColor;
                } else {
                    color = button->normalColor;
                }
                if (button->state.hovered || stayClear) {
                    color = interpColor(color, BLACK, 0.125f);
                }
                onlyDrawButton(button, color);
            } else if (id == 2) {
                if (button->state.pressed || (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_SHIFT)
                                              && KEY_DOWN_FOREGROUND(getHWnd(), 'F'))){
                    color = button->pressedColor;
                } else {
                    color = button->normalColor;
                }
                if (button->state.hovered || stayStart) {
                    color = interpColor(color, BLACK, 0.125f);
                }
                onlyDrawButton(button, color);
            } else if (id == 3) {
                if (button->state.pressed || (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_SHIFT)
                                              && KEY_DOWN_FOREGROUND(getHWnd(), 'C'))) {
                    color = button->pressedColor;
                } else {
                    color = button->normalColor;
                }
                if (button->state.hovered || stayCopy) {
                    color = interpColor(color, BLACK, 0.125f);
                }
                onlyDrawButton(button, color);
            }
        } else {
            setcolor(BLACK);
            settextjustify(CENTER_TEXT, CENTER_TEXT);
        }
    }
    return;
}

int searchButton(int x, int y) {
    int buttonId = false;
    for (int i = 0; i < 3; i++) {
        unsigned int btnId = BUTTON_MIN_ID + i;
        circleButton *button = findButton(btnId);
        if (insideButton(button, x, y)) {
            buttonId = btnId;
            break;
        }
    }
    return buttonId;
}

void drawAllButtons() {
    for (int i = 0; i < 3; i++) {
        unsigned int btnId = BUTTON_MIN_ID + i;
        drawButton(findButton(btnId), btnId);
    }
    return;
}

void draw(PIMAGE pImage) {
    delay_ms(0);
    putimage(0, 0, pImage);
    setfont(24, 0, newTypeface);
    rectprintf(50, 90, 30, 200, "明文");
    rectprintf(50, 237, 30, 200, "字体");
    rectprintf(280, 225, 30, 200, "加密率");
    rectprintf(50, 322, 30, 200, "秘钥");
    rectprintf(50, 467, 30, 200, "密文");
    rectprintf(200, 382, 300, 200, "快捷键: Ctrl+shift");
    xyprintf(476, 382, "+X");
    xyprintf(636, 382, "+F");
    xyprintf(796, 382, "+C");
    plainBox.setfont(23, 0, newTypeface);
    typefaceBox.setfont(23, 0, newTypeface);
    keyBox.setfont(23, 0, newTypeface);
    effBox.setfont(23, 0, newTypeface);
    chiperBox.setfont(23, 0, newTypeface);
    drawAllButtons();
    setcolor(BLACK);
    setfont(24, 0, newTypeface);
    settextjustify(LEFT_TEXT, TOP_TEXT);
    xyprintf(240, 360, "", pressButtonId);
    return;
}

void initButtonState(circleButton *button) {
    button->state.enable = true;
    button->state.visible = true;
    button->state.pressed = false;
    button->state.checked = false;
    button->state.hovered = false;
    return;
}

void initButton(circleButton *button, int radius, int id) {
    int x = 650;
    int y = 302;
    int inter = 160;
    if (button != NULL) {
        if (id == 1) {
            button->text = "清除";
            button->normalColor = EGEARGB(0xFF, 170, 33, 22);
            button->pressedColor = EGEARGB(0xFF, 150, 13, 2);
            button->x = x - inter;
            button->y = y;
            button->radius = radius;
        } else if (id == 2) {
            button->text = "翻译";
            button->normalColor = EGEARGB(0xFF, 64, 224, 208);
            button->pressedColor = EGEARGB(0xFF, 44, 204, 188);
            button->x = x;
            button->y = y;
            button->radius = radius;
        } else if (id == 3) {
            button->text = "复制";
            button->normalColor = EGEARGB(0xFF, 66, 106, 179);
            button->pressedColor = EGEARGB(0xFF, 46, 86, 159);
            button->x = x + inter;
            button->y = y;
            button->radius = radius;
        }
        initButtonState(button);
    }
    return;
}

void initAllButtons() {
    for (int i = 0; i < 3; ++i) {
        int id = BUTTON_MIN_ID + i;
        initButton(findButton(id), 55, id);
    }
    circleButton *btn;
    findButton(BUTTON_MIN_ID + 1);
    findButton(BUTTON_MIN_ID + 2);
    btn = findButton(BUTTON_MIN_ID + 3);
    btn->state.pressed = true;
    return;
}

string moveStringDecode(string moveStringStr) {
    string moveStringRet;
    int i;
    for (i = 0; i < moveStringStr.size(); i += 4) {
        moveStringRet += table[moveStringStr[i]] << 2 | table[moveStringStr[i + 1]] >> 4;
        moveStringRet += (table[moveStringStr[i + 1]] & 0x0f) << 4 | table[moveStringStr[i + 2]] >> 2;
        moveStringRet += table[moveStringStr[i + 2]] << 6 | table[moveStringStr[i + 3]];
    }
    return moveStringRet;
}

string moveStringEncode(string moveStringStr) {
    string moveStringRet;
    int i;
    for (i = 0; i + 3 <= moveStringStr.size(); i += 3) {
        moveStringRet += moveString[moveStringStr[i] >> 2];
        moveStringRet += moveString[(moveStringStr[i] & 0x03) << 4 | moveStringStr[i + 1] >> 4];
        moveStringRet += moveString[(moveStringStr[i + 1] & 0x0f) << 2 | moveStringStr[i + 2] >> 6];
        moveStringRet += moveString[moveStringStr[i + 2] & 0x3f];
    }
    return moveStringRet;
}

void moveStringInit() {
    for (int i = 0; i < 26; i++) moveString[i] = 'A' + i;
    for (int i = 0; i < 26; i++) moveString[26 + i] = 'a' + i;
    for (int i = 0; i < 10; i++) moveString[52 + i] = '0' + i;
    moveString[62] = ' ';
    for (int i = 0; i < 256; i++) table[i] = 0xff;
    for (int i = 0; i < 64; i++) table[moveString[i]] = i;
    return;
}

string moveStringMain(bool opt, string moveStringStr) {
    if (opt) {
        return moveStringEncode(moveStringStr);
    } else {
        return moveStringDecode(moveStringStr);
    }
}

void confusion(string *pEnString) {
    for (int i = 0; i < (rand() % iEff) * (double)(iEff / 9) * 2; ++i) {
        if (rand() % 2) {
            pEnString->push_back('A' + 13 + rand() % 13);
        } else {
            pEnString->push_back('a' + rand() % 13);
        }
    }
    return;
}

void ten2Two(int x, vector<char> *basicVec) {
    bool isNegative = false;
    if (x < 0) {
        x = -x;
        isNegative = true;
    }
    for (; x != 1;) {
        int temp = x % 2;
        char t;
        if (temp == 1) {
            t = 'A' + rand() % 13;
        } else {
            t = 'a' + 13 + rand() % 13;
        }
        basicVec->push_back(t);
        x -= temp;
        x /= 2;
    }
    basicVec->push_back('A' + rand() % 13);
    if (isNegative) {
        basicVec->push_back('a' + 13 + rand() % 13);
    }
    reverse(basicVec->begin(), basicVec->end());
    return;
}

bool getIsNegative(const string &basicString) {
    bool isNegative = false;
    for (int i = 0; i < basicString.length(); ++i) {
        if ('A' + 13 <= basicString[i] && basicString[i] < 96) {
            continue;
        }
        if ('a' <= basicString[i] && basicString[i] < 'a' + 13) {
            continue;
        }
        if (basicString[i] >= 'a') {
            isNegative = true;
        }
        break;
    }
    return isNegative;
}

int distinction(string basicString) {
    int times = -1;
    int distinctionRes = 0;
    bool isNegative;
    isNegative = getIsNegative(basicString);
    for (int i = basicString.length() - 1; i >= 0 + isNegative; --i) {
        if ('A' + 13 <= basicString[i] && basicString[i] < 96) {
            continue;
        }
        if ('a' <= basicString[i] && basicString[i] < 'a' + 13) {
            continue;
        }
        times++;
        if (basicString[i] <= 90) {
            distinctionRes = distinctionRes + pow(2, times);
        }
    }
    if (isNegative) {
        distinctionRes = -distinctionRes;
    }
    return distinctionRes;
}

void
initCode(char *capitalEncoder, char *capitalDecoder, char *lowEncoder, char *lowDecoder, int sK, string &s1, int bK) {
    for (int i = 0; i < s1.length(); ++i) {
        s1[i] = abs(s1[i] - '0') % 26 + 'a';
    }
    for (int i = 0; i < s1.length(); ++i) {
        s1[i] = (int) ((s1[i] - 'a') * pow(s1.length(), 2)) % 26 + 'a';
    }
    set<char> ss;
    for (int i = 0; i < s1.length(); i++) {
        ss.insert(s1[i]);
    }
    s1 = "";
    set<char>::iterator it;
    for (it = ss.begin(); it != ss.end(); it++) {
        if (*it == ' ') break;
        s1.push_back(*it);
    }
    for (int i = 0; i < s1.length(); ++i) {
        capitalEncoder[i] = s1[i] - 32;
        lowEncoder[i] = s1[i];
    }
    for (int i = 0; i < 26; ++i) {
        if (lowEncoder[i] != 0) {
            ++sK;
        }
        if (capitalEncoder[i] != 0) {
            ++bK;
        }
    }
    for (char x = 'A'; x <= 'Z'; ++x) {
        bool flag = true;
        for (int i = 0; i < 26; ++i) {
            if (capitalEncoder[i] == x) {
                flag = false;
                break;
            }
        }
        if (flag) {
            capitalEncoder[bK] = x;
            ++bK;
        }
    }
    for (char x = 'a'; x <= 'z'; ++x) {
        bool flag = true;
        for (int i = 0; i < 26; ++i) {
            if (lowEncoder[i] == x) {
                flag = false;
                break;
            }
        }
        if (flag) {
            lowEncoder[sK] = x;
            ++sK;
        }
    }
    for (int i = 0; i < 26; ++i) {
        capitalDecoder[capitalEncoder[i] - 'A'] = i + 'A';
    }
    for (int i = 0; i < 26; ++i) {
        lowDecoder[lowEncoder[i] - 'a'] = i + 'a';
    }
    return;
}

string keyCoder(bool op, string basicString) {
    char capitalEncoder[28] = {0};
    char capitalDecoder[28];
    char lowEncoder[28] = {0};
    char lowDecoder[28];
    memset(capitalEncoder, 0, sizeof(capitalEncoder));
    memset(capitalDecoder, 0, sizeof(capitalDecoder));
    memset(lowEncoder, 0, sizeof(lowEncoder));
    memset(capitalDecoder, 0, sizeof(capitalDecoder));
    int sK = 0;
    string s1;
    s1 = newKey;
    newKey = "";
    int bK = 0;
    initCode(capitalEncoder, capitalDecoder, lowEncoder, lowDecoder, sK, s1, bK);
    return edCode(op, basicString, capitalEncoder, capitalDecoder, lowEncoder, lowDecoder);
}

string
edCode(bool op, string &st, const char *capitalEncoder, const char *capitalDecoder, const char *lowEncoder,
       const char *lowDecoder) {
    if (op) {
        for (int i = 0; i < st.length(); ++i) {
            if (st[i] >= 'a' && st[i] <= 'z') {
                st[i] = lowDecoder[st[i] - 'a'];
            } else if (st[i] >= 'A' && st[i] <= 'Z') {
                st[i] = capitalDecoder[st[i] - 'A'];
            }
        }
    } else {
        for (int i = 0; i < st.length(); ++i) {
            if (st[i] >= 'a' && st[i] <= 'z') {
                st[i] = lowEncoder[st[i] - 'a'];
            } else if (st[i] >= 'A' && st[i] <= 'Z') {
                st[i] = capitalEncoder[st[i] - 'A'];
            }
        }
    }
    return st;
}

void encrypt(string basicString) {
    string enRes;
    vector<char> tmpVec;
    for (int j = 0; j < basicString.length(); ++j) {
        int eaInfo = basicString[j];
        ten2Two(eaInfo, &tmpVec);
        for (int i = 0; i < tmpVec.size(); ++i) {
            confusion(&enRes);
            enRes.push_back(tmpVec[i]);
        }
        confusion(&enRes);
        enRes.push_back(' ');
        tmpVec.clear();
    }
    enRes.push_back('A' + rand() % 13);
    confusion(&enRes);
    enRes.push_back('a' + 13 + rand() % 13);
    confusion(&enRes);
    enRes.push_back('a' + 13 + rand() % 13);
    confusion(&enRes);
    enRes.push_back('a' + 13 + rand() % 13);
    confusion(&enRes);
    enRes.push_back('a' + 13 + rand() % 13);
    confusion(&enRes);
    enRes.push_back('a' + 13 + rand() % 13);
    confusion(&enRes); // protect
    enRes = moveStringMain(ENC, enRes);
    enRes = keyCoder(ENC, enRes);
    char tempC[0x3f3f];
    strcpy(tempC, enRes.c_str());
    chiperBox.settext(tempC);
    return;
}

void decrypt(string chiperString) {
    string dePartString;
    string deRes;
    chiperString = keyCoder(DEC, chiperString);
    chiperString = moveStringMain(DEC, chiperString);
    int j = 0;
    for (int i = 0; i < chiperString.length(); ++i) {
        if (chiperString[i] == ' ') {
            dePartString = chiperString.substr(j, i - j);
            j = i + 1;
            deRes.push_back(distinction(dePartString));
        } else if (i == chiperString.length() - 1) {
            dePartString = chiperString.substr(j, i - j + 1);
            deRes.push_back(distinction(dePartString));
        }
    }
    deRes.erase(deRes.end() - 1);
    char tempC[0x3f3f];
    strcpy(tempC, deRes.c_str());
    plainBox.settext(tempC);
    return;
}

void initWindow() {
    setcaption("ED");
    initgraph(940, 630, INIT_RENDERMANUAL);

    ege_enable_aa(true);
    setbkcolor(RGB(255, 255, 255));
    setfont(20, 0, newTypeface);
    setbkmode(TRANSPARENT);

    plainBox.create(true);
    plainBox.move(100, 35);

    keyBox.create(true);
    keyBox.move(100, 332);

    typefaceBox.create(true);
    typefaceBox.move(100, 247);

    effBox.create(true);
    effBox.move(330, 247);

    chiperBox.create(true);
    chiperBox.move(100, 417);

    plainBox.setfont(23, 0, newTypeface);
    typefaceBox.setfont(23, 0, newTypeface);
    keyBox.setfont(23, 0, newTypeface);
    effBox.setfont(23, 0, newTypeface);
    chiperBox.setfont(23, 0, newTypeface);

    plainBox.size(784, 162);
    plainBox.setbgcolor(RGB(238, 238, 238));
    plainBox.setcolor(RGB(0, 0, 0));
    plainBox.setfont(23, 0, newTypeface);

    keyBox.size(290, 31);
    keyBox.setbgcolor(RGB(238, 238, 238));
    keyBox.setcolor(RGB(0, 0, 0));
    char tempC1[0x3f3f];
    strcpy(tempC1, oldKey.c_str());
    keyBox.settext(tempC1);

    typefaceBox.size(140, 31);
    typefaceBox.setbgcolor(RGB(238, 238, 238));
    typefaceBox.setcolor(RGB(0, 0, 0));
    typefaceBox.settext(oldTypeface);

    effBox.size(60, 31);
    effBox.setbgcolor(RGB(238, 238, 238));
    effBox.setcolor(RGB(0, 0, 0));
    char tempC[0x3f3f];
    strcpy(tempC, oldSEff.c_str());
    effBox.settext(tempC);

    chiperBox.size(784, 162);
    chiperBox.setbgcolor(RGB(238, 238, 238));
    chiperBox.setcolor(RGB(0, 0, 0));

    plainBox.setmaxlen(MAX_LEN);
    plainBox.visible(true);
    keyBox.setmaxlen(MAX_LEN);
    keyBox.visible(true);
    keyBox.setmaxlen(26);
    typefaceBox.setmaxlen(MAX_LEN);
    typefaceBox.visible(true);
    effBox.setmaxlen(MAX_LEN);
    effBox.visible(true);
    effBox.setmaxlen(1);
    chiperBox.setmaxlen(MAX_LEN);
    chiperBox.visible(true);
    plainBox.setfocus();
    initAllButtons();
    ege_enable_aa(true);
    return;
}

void ed(string &plainString, string &chiperString) {
    if (isWrongEff()) {
        MessageBox(GetForegroundWindow(),
                   TEXT("请在“加密率”一栏中输入一个1-9的整数"),
                   TEXT("说明ED"), MB_ICONERROR | MB_OK);
        effBox.settext("2");
        return;
    }
    string s = newTypeface;
    if (s == "") {
        typefaceBox.settext("宋体");
        MessageBox(GetForegroundWindow(),
                   TEXT("请在“字体”一栏中输入合法字符"),
                   TEXT("说明ED"), MB_ICONERROR | MB_OK);
        return;
    }
    if (chiperString.empty() && !plainString.empty()) { // Encrypt
        encrypt(plainString);
        plainBox.settext("");
        chiperBox.setfocus();
    } else if (!chiperString.empty() && plainString.empty()) { // Decrypt
        decrypt(chiperString);
        chiperBox.settext("");
        plainBox.setfocus();
    } else {
        MessageBox(GetForegroundWindow(),
                   TEXT("请在一栏内输入"),
                   TEXT("说明ED"), MB_ICONERROR | MB_OK);
    }
    willStart = false;
    return;
}

void mouse(int &xMouse, int &yMouse, bool &mouseMoved) {
    while (mousemsg()) {
        mouse_msg msg = getmouse();
        searchButton(msg.x, msg.y);
        if (msg.is_left()) {
            if (msg.is_down()) {
                int btnId;
                btnId = searchButton(msg.x, msg.y);
                if (btnId == 1) {
                    clickClear = true;
                    stayClear = true;
                } else if (btnId == 2) {
                    clickStart = true;
                    stayStart = true;
                } else if (btnId == 3) {
                    clickCopy = true;
                    stayCopy = true;
                }
                if ((btnId != 0) && (findButton(btnId)->state.enable)) {
                    pressButtonId = btnId;
                    findButton(pressButtonId)->state.pressed = true;
                }
            } else {
                stayStart = false;
                stayClear = false;
                stayCopy = false;
                int btnId1;
                btnId1 = searchButton(msg.x, msg.y);
                if (btnId1 == 1) {
                    willClear = true;
                } else if (btnId1 == 2) {
                    willStart = true;
                } else if (btnId1 == 3) {
                    willCopy = true;
                }
                if (pressButtonId != 0) {
                    unsigned btnId = pressButtonId;
                    circleButton *button = findButton(btnId);
                    button->state.pressed = false;
                    pressButtonId = 0;
                    button->state.checked = !(button->state.checked);
                }
            }
        } else if (msg.is_move()) {
            mouseMoved = true;
            xMouse = msg.x;
            yMouse = msg.y;
        }
    }
    return;
}

bool isWrongEff() {
    if (newSEff.length() > 1) {
        return true;
    } else if (!((newSEff[0] - '0' > 0) && (newSEff[0] - '0' <= 9))) {
        if (newSEff[0] == 0) {
            effBox.settext("2");
            return false;
        } else {
            return true;
        }
    } else {
        iEff = newSEff[0] - '0';
        return false;
    }
}

void
setString(const int buffSize, char *strBuffer, char *strBuffer1,
          char *strBuffer2, char *strBuffer3, char *strBuffer4, string &plainString, string &chiperString) {
    setfont(24, 0, newTypeface);
    plainBox.gettext(buffSize, strBuffer);
    chiperBox.gettext(buffSize, strBuffer1);
    keyBox.gettext(buffSize, strBuffer2);
    effBox.gettext(buffSize, strBuffer3);
    typefaceBox.gettext(buffSize, strBuffer4);
    plainString = strBuffer;
    chiperString = strBuffer1;
    newSEff = strBuffer3;
    newKey = strBuffer2;
    newTypeface = strBuffer4;
    return;
}

void operations(string &s, string &s1, ofstream &file_writer) {
    if (willStart && clickStart) {
        changeHistory(file_writer);
        oldKey = newKey;
        willStart = false;
        clickStart = false;
        ed(s, s1);
    }
    if (willClear && clickClear) {
        chiperBox.settext("");
        plainBox.settext("");
        willClear = false;
        clickClear = false;
    }
    if (willCopy && clickCopy) {
        char tempC[0x3f3f];
        if (s == "") {
            strcpy(tempC, s1.c_str());
            changeClipboard(tempC);
        } else if (s1 == "") {
            strcpy(tempC, s.c_str());
            changeClipboard(tempC);
        } else {
            MessageBox(GetForegroundWindow(),
                       TEXT("复制时必须一栏为空"),
                       TEXT("说明ED"), MB_ICONERROR | MB_OK);
        }
        willCopy = false;
        clickCopy = false;
    }
    return;
}

void changeHistory(ofstream &file_writer) {
    if (!newTypeface || !*newTypeface) {
        file_writer << "IAmAdmin" << endl;
    } else {
        file_writer << newTypeface << endl;
    }
    if (newSEff == "") {
        file_writer << "IAmAdmin" << endl;
    } else {
        file_writer << newSEff << endl;
    }
    if (newKey == "") {
        file_writer << "IAmAdmin" << endl;
    } else {
        file_writer << newKey << endl;
    }
    oldSEff = newSEff;
    oldTypeface = newTypeface;
    oldKey = newKey;
}

void
running(const int buffSize, char *strBuffer, char *strBuffer1, char *strBuffer2, char *strBuffer3, char *strBuffer4,
        int xMouse, int yMouse, ofstream &file_writer) {
    leaveStart = true;
    int wrongFlag1 = 0;
    PIMAGE pImage = newimage();
    getimage(pImage, "bk.jpeg");
    int flushFlag = 0;
    for (; is_run(); delay_fps(60)) {
        if ((GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_SHIFT)
             && KEY_DOWN_FOREGROUND(getHWnd(), 'X'))) {
            willClear = true;
            clickClear = true;
        }
        if ((GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_SHIFT)
             && KEY_DOWN_FOREGROUND(getHWnd(), 'F'))) {
            leaveStart = false;
        } else {
            if (!leaveStart) {
                willStart = true;
                clickStart = true;
            }
            leaveStart = true;
        }
        if ((GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_SHIFT)
             && KEY_DOWN_FOREGROUND(getHWnd(), 'C'))) {
            willCopy = true;
            clickCopy = true;
        }
        string plainString;
        string chiperString;
        setString(buffSize, strBuffer, strBuffer1, strBuffer2, strBuffer3,
                  strBuffer4, plainString, chiperString);

        if (flushFlag % 300 == 0) {
            changeHistory(file_writer);
        }
        flushFlag++;
        bool mouseMoved = false;
        operations(plainString, chiperString, file_writer);
        mouse(xMouse, yMouse, mouseMoved);
        if (mouseMoved) {
            int btnId = searchButton(xMouse, yMouse);
            if (btnId != hoveredButtonId) {
                if (hoveredButtonId != 0) {
                    findButton(hoveredButtonId)->state.hovered = false;
                }
                if (btnId != 0) {
                    findButton(btnId)->state.hovered = true;
                }
                hoveredButtonId = btnId;
            }
        }
        draw(pImage);
    }
}
