/*
输入二型文法以及token表，返回四元式

*/
#include<bits/stdc++.h>
#include"cpp.cpp"
using namespace std;
/*--------全局变量以及结构体-----------*/
int k=0;
struct Quaternary{
    string op;
    string arg1;
    string arg2;
    string result;
};
struct Token{
    int line;
    string word;
    int type;
};
struct SignTable{
    //符号表
    string name="";//变量名称
    string type;
    string val;
};

//查询符号表，返回name对应的记录
int lookup(string name,vector<SignTable>signtable){
    for(int i=0;i<signtable.size();i++){
        if(name==signtable[i].name){
            return i;
        }
    }
    
    return -1;

}
//返回一个新的临时变量
string newtemp(){
    string p="t";
    p+=to_string(k);
    k++;
    return p;
}
//生成四元式
Quaternary emit(string result,string arg1,string arg2,string op){
    
    Quaternary tquad;
    tquad.arg1=arg1;
    tquad.arg2=arg2;
    tquad.result=result;
    tquad.op=op;
    return tquad;
}
//花式报错
void ERROR(int type,string name){
    switch(type){
        case 0:
            //重复定义
            cout<<"\033[31m Failed\t:"<<name<<"\tis redefined\033[0m"<<endl;
            break;
        case 1:
            //未被赋值
            cout<<"\033[31m Failed\t:"<<name<<"\tis not assigned\033[0m"<<endl;
            break;
        case 2:
            //未被定义值就参与运算
            cout<<"\033[31m Failed\t:"<<name<<"\tdoes not has a value\033[0m"<<endl;
            break;
        case 3:
            //未被定义
            cout<<"\033[31m Failed\t:"<<name<<"\tis not defined\033[0m"<<endl;
            break;

    }
}
//乘法
SignTable MUL(string a,string b){
    SignTable temp;
    temp.name= newtemp();
    string ans;
    //TODO:这边需要加的是不同类型之间的乘法
    cout<<"*"<<a<<" "<<b<<endl;
    ans=to_string(stoi(a)*stoi(b));
    temp.val=ans;
    return temp;
}
//加法
SignTable ADD(string a,string b){
    SignTable temp;
    temp.name= newtemp();
    string ans;
    //TODO:这边需要加的是不同类型之间的加法
    cout<<"+"<<a<<" "<<b<<endl;
    ans=to_string(stoi(a)+stoi(b));
    temp.val=ans;
    return temp;
    
}
Quaternary meaning_process(int id,stack<Token>&tokens,vector<SignTable>&signtable,bool &has_err){
    Quaternary temp;
    temp.op="";
    if(has_err){
        return temp;
    }
    switch(id){
        case 7:{
            //[FORMULA] --> ['int'][sign]定义语句
            tokens.pop();
            string name=tokens.top().word;
            tokens.pop();
            string type=tokens.top().word;
            cout<<"7:"<<name<<" "<<type<<endl;
            tokens.pop();
            int p=lookup(name,signtable);
            if(p!=-1){
                //说明之前被注册过了
                has_err=true;
                ERROR(0,name);
            }else{
                //之前没有注册，现在注册
                SignTable ns;
                ns.name=name;
                ns.type=type;
                signtable.push_back(ns);
            }
            break;
        }
        case 12:{
            //赋值语句中的一些：[N] --> [sign]
            
            break;
        }
        case 10:{
            //[E] --> [N]['*'][E]
            tokens.pop();
            Token arg1=tokens.top();
            tokens.pop();
            tokens.pop();
            Token arg2=tokens.top();
            tokens.pop();
            bool flag=true;
            if(arg1.type!=3){
                int p=lookup(arg1.word,signtable);
                if(p==-1){
                    //如果没有注册就参与运算
                    has_err=true;
                    ERROR(2,arg1.word);
                    flag=false;
                }else if(signtable[p].val==""){
                    //如果注册了但是没有赋值就参与运算
                    has_err=true;
                    ERROR(1,arg1.word);
                    flag=false;
                }

            }
            if(arg2.type!=3){
                int p=lookup(arg2.word,signtable);
                if(p==-1){
                    //如果没有注册就参与运算
                    has_err=true;
                    ERROR(2,arg2.word);
                    flag=false;
                }else if(signtable[p].val==""){
                    //如果注册了但是没有赋值就参与运算
                    has_err=true;
                    ERROR(1,arg2.word);
                    flag=false;
                }

            }
            if(flag){
                string a,b;
                if(arg1.type==3){
                    a=arg1.word;
                }else{
                    int p=lookup(arg1.word,signtable);
                    a=signtable[p].val;
                }
                if(arg2.type==3){
                    b=arg2.word;
                }else{
                    int p=lookup(arg2.word,signtable);
                    b=signtable[p].val;
                }
                SignTable res=MUL(a,b);
                signtable.push_back(res);
                Token nt;
                nt.word=res.name;
                nt.type=4;
                tokens.push(nt);
                temp=emit(res.name,arg1.word,arg2.word,"*");
            }

            break;
        }
        case 8:{
            //[F] --> [E]['+'][F]
            //TODO：这边有点问题，继续规约的时候，不可以直接去除规约符号
            
            if(tokens.top().word==";"){
                tokens.pop();
            }
            Token arg1=tokens.top();
            tokens.pop();
            tokens.pop();
            Token arg2=tokens.top();
            tokens.pop();
            bool flag=true;
            if(arg1.type!=3){
                int p=lookup(arg1.word,signtable);
                if(p==-1){
                    //如果没有注册就参与运算
                    has_err=true;
                    ERROR(2,arg1.word);
                    flag=false;
                }else if(signtable[p].val==""){
                    //如果注册了但是没有赋值就参与运算
                    ERROR(1,arg1.word);
                    has_err=true;
                    flag=false;
                }

            }
            if(arg2.type!=3){
                int p=lookup(arg2.word,signtable);
                if(p==-1){
                    //如果没有注册就参与运算
                    has_err=true;
                    ERROR(2,arg2.word);
                    flag=false;
                }else if(signtable[p].val==""){
                    //如果注册了但是没有赋值就参与运算
                    has_err=true;
                    ERROR(1,arg2.word);
                    flag=false;
                }

            }
            if(flag){
                string a,b;
                if(arg1.type==3){
                    a=arg1.word;
                }else{
                    int p=lookup(arg1.word,signtable);
                    a=signtable[p].val;
                }
                if(arg2.type==3){
                    b=arg2.word;
                }else{
                    int p=lookup(arg2.word,signtable);
                    b=signtable[p].val;
                }
                
                SignTable res=ADD(a,b);
                signtable.push_back(res);
                Token nt;
                nt.word=res.name;
                nt.type=4;
                tokens.push(nt);
                temp=emit(res.name,arg1.word,arg2.word,"+");
            }


            break;
        }
        case 6:{
            //[FORMULA] --> [sign]['='][F]赋值语句
            if(tokens.top().word==";"){
                tokens.pop();
            }
            
            string val=tokens.top().word;
            
            tokens.pop();
            string a=tokens.top().word;
            tokens.pop();
            string name=tokens.top().word;
            cout<<name<<"|"<<val<<"|"<<a<<endl;
            tokens.pop();
            int p=lookup(name,signtable);
            if(p==-1){
                has_err=true;
                ERROR(3,name);
            }else{
                //注册值或者更改值
                signtable[p].val=val;
                temp=emit(val,name,"_","=");
            }
            break;
        }
        case 11:{
           //[E] --> [N]

            break;
        }
        case 9:{
            //[F] --> [E]
            
            break;
        }
        default:{
            //看语法，grammar前面有几个就打几个7 12 10 8 6
            break;
        }
    }
    return temp;
}
