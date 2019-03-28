/*
* Copyright (C) 2019, Andrew Yachmenyov
*/
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

vector<int> PlaceNonTerminals;
vector<string> LeftRule, RightRule, FinalChains;
int lowerLimit, upperLimit;

string substitution(string s, string change, int entry, int lengthOfSubstr)
{
    string result = "";
    for (int i = 0; i<entry; i++)
    {
        result+= s[i];
    }
    result += change;
    for (int i = entry+lengthOfSubstr; i<s.length(); i++)
    {
        result+=s[i];
    }
    return result;
}

void partition(string str, string substr, vector<int>* entries)
{
    int a;
    while (true)
    {
        a = (int)str.find(substr);
        if (a == -1)
        {
           break;
        }
        entries->push_back(a);
        str[a] = '>';
    }
}

void partition(char* str, char* substr, vector<string>* v)
{
    if (string(str) == string(substr))
    {
         return;
    }
    else
    {
        char *p;
        p = strtok(str, substr);
        v->push_back(string(p));
        
        while (p!=NULL)
        {
            p = strtok(NULL, substr);
            if (p!= NULL)
            {
                v->push_back(string(p));
            }
        }
        return;
    }
}

int typeDefinition(string terminals)
{
    int currentType = 0;
    bool flag = false;
    /*Type 3*/
    for (size_t i = 0; i < LeftRule.size(); i++)
    {
        flag = false;
        if (LeftRule[i].size() == 1)
        {
            int counter = 0;
            for (char ch: RightRule[i])
            {
                if (ch >= 65 && ch <=90)
                {
                    counter++;
                }
            }
            if (counter < 2 || RightRule[i] == "&")
            {
                flag = true;
            }
            else break;
        }
        else break;
    }
    if (flag)
    {
        return 3;
    }

    /*Type 2*/

    for (size_t i = 0; i < LeftRule.size(); i++)
    {
        flag = false;
        if (LeftRule[i].size() == 1)
        {
            flag = true;
        }
        else
        {
            break;
        }
    }
    if (flag)
    {
        return 2;
    }

    /*Type 1*/

    for (size_t i = 0; i < LeftRule.size(); i++)
    {
        flag = false;
        if (RightRule[i] != "&" && LeftRule[i].size() <= RightRule[i].size())
        {
            flag = true;
        }
        else
        {
            break;
        }
    }
    if (flag)
    {
        return 1;
    }
    else
    {
        return 0; /*Type 0*/
    }
    
}
void prodactive(string terminals, vector<string>* RProd, vector<string>* LProd) //for type 2 grammar
{
    string N = terminals, P = "";
    string PreviousP = "n";
    while (P != PreviousP)
    {
        RProd->clear();
        LProd->clear();
        for (int i = 0; i < RightRule.size(); i++)
        {
            string f = RightRule[i];
            int counter = 0;
            PreviousP = P;
            for (char ch: RightRule[i])
            {
                char ind = ch;
                for (char n: N)
                {
                    char ind2 = n;
                    if (ch == n)
                    {
                        counter++;

                    }
                }
            }
            if (counter == RightRule[i].length())
            {
                if ((int)P.find(LeftRule[i]) == -1)
                {
                    P += LeftRule[i];
                }
                if ((int)N.find(LeftRule[i]) == -1)
                {
                    N += LeftRule[i];
                }
                RProd->push_back(RightRule[i]);
                LProd->push_back(LeftRule[i]);
            }
        }
    }
    
}

void reachable(string nonTerminals, vector<string>* RRich, vector<string>* LRich,
vector<string> RightRule, vector<string> LeftRule)
{
    string W = LeftRule[0];

    for (int i = 0; i < W.length(); i++)
    {
        for (int j = 0; j < LeftRule.size(); j++)
        {
            if (LeftRule[j][0] == W[j])
            {
                for (char ch: RightRule[i])
                {
                    for (char n: nonTerminals)
                    {
                        if (ch == n && (int)W.find(ch) == -1)
                        {
                            W+=ch;
                        }
                    }
                }
            }
        }
        
    }

    for (int i = 0; i < LeftRule.size(); i++)
    {
        for (char ch: W)
        {
            if (LeftRule[i][0] == ch)
            {
                LRich->push_back(LeftRule[i]);
                RRich->push_back(RightRule[i]);
            }
        }
    }
}

string del_char(string str, int index)
{
    string buf = "";
    for (int i = 0; i < str.length(); i++)
    {
        if(i != index)
        {
            buf += str[i];
        }
    }
    return buf;
}

void permutation(string set, vector<string>* perm)
{
    if(set.length() != 1)
    {
        for(int i = 0; i < set.length(); i++)
        {
            string str = del_char(set, i);
            perm->push_back(str);
            permutation(str, perm);
        }  
    }
    else
    {
        perm->push_back(set);
    }  
}

string epsRules()
{
    string N = "&", P = "";
    string PreviousP = "n";
    vector<string> LRule, Rrule;
    while (P != PreviousP)
    {
        for (int i = 0; i < RightRule.size(); i++)
        {
            string f = RightRule[i];
            PreviousP = P;
            for (char ch: RightRule[i])
            {
                char ind = ch;
                for (char n: N)
                {
                    char ind2 = n;
                    if (ch == n)
                    {
                        if ((int)P.find(LeftRule[i]) == -1)
                        {
                            P += LeftRule[i];
                        }
                        if ((int)N.find(LeftRule[i]) == -1)
                        {
                            N += LeftRule[i];
                            LRule.push_back(LeftRule[i]);
                            Rrule.push_back(RightRule[i]);
                        }
                    }
                }
            }
        }
    }

    vector<string> PLRule, PRrule;

    for (int i = 0; i < RightRule.size(); i++)
    {
        if (RightRule[i] != "&")
        {
            PLRule.push_back(LeftRule[i]);
            PRrule.push_back(RightRule[i]);
        }
    }
    
    for (int i = 0; i < RightRule.size(); i++)
    {
        string entry_matrix = "";
        vector<int> entry_matrix;
        for (int j = 0; j < RightRule[i].length(); j++)
        {
            for(char n: P)
            {
                if(RightRule[i][j] == n)
                {
                    entry_matrix.push_back(j);
                }
            }
        }
        
        vector<string> permutations, sifted_perm;
        bool  perm_used [permutations.size()];
        permutations.push_back(entry_matrix);

        permutation(entry_matrix, &permutations);

        for(int k = 0; k < permutations.size(); k++)
        {
            string bufi = permutations[i];
            int counter = 0;
            for(int j = 0; j < permutations.size(); j++)
            {
                string bufj = permutations[j];
                if(permutations[k] == permutations[j])
                {
                    counter++;
                    if(counter == 1 && perm_used[j] == false)
                    {
                        sifted_perm.push_back(permutations[j]);
                        perm_used[j] = true;
                    }
                    if(counter > 1)
                    {
                        perm_used[j] = true;
                    }  
                }     
            }
        /*there is a need to convert the enty_matrix from string to vector<int>*/
        }
    }
    

    return P;
}

int findChains(string chain, string terminal, string nonTerminal)
{
    if (chain.length() > upperLimit)
    {
        return 0;
    }
    bool Allterminals = true;
    for (char i: chain)
    {
        for (char j: nonTerminal)
        {
            if (i == j)
            {
                Allterminals = false;
                break;
            }
        }
    }

    if (Allterminals)
    {
        if (chain.length() > 1)
        {
            while (true)
            {
                int m = chain.find("&");
                if (m == -1)
                {
                    break;
                }
                else
                {
                    chain = substitution(chain, "", m, 1);
                }
            }
        }
        FinalChains.push_back(chain);
        if (FinalChains.size() == 68150)
        {
            int p = 0;
        }
        return 0;
    }
    
    vector<string>entLRules;
    vector<string> entRRules;
    int min = chain.length();

    for (int i = 0; i < LeftRule.size(); i++)
    {
        int a = (int)chain.find(LeftRule[i]);
        if (a!=-1)
        {
           if (a < min)
           {
                entLRules.clear();
                entRRules.clear();
                entLRules.push_back(LeftRule[i]);
                entRRules.push_back(RightRule[i]);
                min = a;
           }
           else if (a == min)
           {
                entLRules.push_back(LeftRule[i]);
                entRRules.push_back(RightRule[i]);
           }
        }
    }

    for (int i = 0; i < entLRules.size(); i++)
    {
        string bufChain = chain;
        bufChain = substitution(chain, entRRules[i], chain.find(entLRules[i]),
        entLRules[i].length());
        findChains(bufChain, terminal, nonTerminal);
    }
}

int main(int argc, char* argv[])
{

    vector<string> v;
    string nonTerminal, terminal;

    cout<<"Введите нетерминальные символы"<<endl;
    cin>>nonTerminal;
    cout<<"Введите терминальные символы"<<endl;
    cin>>terminal;
    cout<<"Введите грамматику в формате: S>Sa. Пустая цепочка обозначается символом: &."<<endl;
    cin.clear();
    while (cin.get() != '\n')
    {
    }
    string s = "";
    getline(cin, s);
    cout<<"Введите нижнюю границу длины цепочек:"<<endl;
    cin>>lowerLimit;
    cout<<"Введите верхнюю границу длины цепочек:"<<endl;
    cin>>upperLimit;
    

    partition((char*)s.c_str(), (char*)" ", &v);

    char * buf;
    for (string n: v)
    {
        buf = strtok((char*)n.c_str(), ">");
        LeftRule.push_back(buf);
        while (buf != NULL)
        {
            buf = strtok(NULL, ">");
            if (buf != NULL)
            {
                RightRule.push_back(buf);
            }
        }
    }
    int type = typeDefinition(terminal);

    cout<<"Grammary type: "<<type<<endl;

    string current = "";
    if (LeftRule.size() != 0)
    {
        current = LeftRule[0];
    }
    else
    {
        cout<<"Правила грамматики заданны некорректо или вовсе не заданы!"<<endl;
        return -1;
    }

    for (string st: LeftRule)
    {
        bool stop = false;
        for (int i = 0; i< st.length(); i++)
        {
            for (char ch: nonTerminal)
            {
                if (st[i] == ch)
                {
                    PlaceNonTerminals.push_back(i);
                    stop = true;
                    break;
                }
            }
            if (stop)
            {
                break;
            }
        }
    }

    findChains(LeftRule[0], terminal, nonTerminal);

    std::ofstream out;          // поток для записи
    out.open("D:\\hello.txt");
    
    cout<<"Цепочки:"<<endl;
    int counter = 1;
    for (string n: FinalChains)
    {
        if (lowerLimit <= n.length() <= upperLimit)
        {
            cout<<counter<<". "<<n<<endl;
            out<<counter<<". "<<n<<endl;
            counter++;
        }
    }


    vector<string> finalch;
    finalch.push_back(FinalChains[0]);
    for (int i = 0; i < FinalChains.size(); i++)
    {
        bool b = false;
        for (int j = 0; j < finalch.size(); j++)
        {
            if (FinalChains[i]==finalch[j])
            {
                b = true;
                break;
            }
        }
        if (b != true)
        {
            finalch.push_back(FinalChains[i]);
        }
    }

    vector<string> LProd, RProd;
    if (type > 1)
    {
        prodactive(terminal, &RProd, &LProd);

        cout<<"Производящие:"<<endl;

        for (int i = 0; i < LProd.size(); i++)
        {
            cout<<LProd[i]<<">"<<RProd[i]<<endl;
        }

         vector<string> LRich, RRich;

        reachable(nonTerminal, &RRich, &LRich, RProd, LProd);

        cout<<"Достижимые:"<<endl;

        for (int i = 0; i < LRich .size(); i++)
        {
            cout<<LRich[i]<<">"<<RRich[i]<<endl;
        }
        cout<<"rules"<<endl;
        for (int i = 0; i < LeftRule.size(); i++)
        {
            cout<<LeftRule[i]<<">"<<RightRule[i]<<endl;
        }

        string strr = epsRules();

        cout<<"eps-rules: "<<strr<<endl;
    }
    return 0;
}
