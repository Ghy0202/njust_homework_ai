#include<bits/stdc++.h>
#include"Meaning.cpp"
using namespace std;
/*
程序有两个输入：1）一个是文本文档，其中包含2?型文法（上下文无关文法）的产生式集合；
2）任务1词法分析程序输出的（生成的）token令牌表。

程序的输出包括：
YES或NO（源代码字符串符合此2?型文法，或者源代码字符串不符合此2?型文法）；
错误提示文件，如果有语法错标示出错行号，并给出大致的出错原因。
*/
const int maxn=100;
const int rsign=-1000;
char buffer[maxn];
const int failed=-INT_MAX;//记录失败

string grammarfile="./data/syn_grammar.txt";
//string tokenfile="./data/syn_token.txt";
string tokenfile="./data/token_table_equal.txt";

vector<Quaternary>quads;//存储四元式
//string tokenfile="token_table.txt";
// string grammarfile="simple_grammar.txt";
// string tokenfile="simple_code.txt";
/*代码使用到的结构体*/
struct Grammar{//从文件中读取的二型文法
    string Left;
    vector<string>Rights;
};
//TODO:error: no match for 'operator<' (operand types are 'const Closure' and 'const Closure')
struct Item{                //项目集中的项目
    string Left;
    vector<string>Rights;
    string Tail;
    int pos=0;              //“.”所在的位置
    //运算符重载
    //TODO:==重新写
    friend bool operator == (Item a,Item b){
        if(a.Left!=b.Left){
            return false;
        }else if(a.Tail!=b.Tail){
            return false;
        }else if(a.pos!=b.pos){
            return false;
        }else if(a.Rights.size()!=b.Rights.size()){
            return false;
        }else{
            for(int i=0;i<a.Rights.size();i++){
                if(a.Rights[i]!=b.Rights[i]){
                    return false;
                }
            }
            return true;
        }
    }
    friend bool operator <(Item a,Item b){
        if(a.Left!=b.Left){
            return a.Left<b.Left;
        }else if(a.pos!=b.pos){
            return a.pos<b.pos;
        }else if(a.Tail!=b.Tail){
            return a.Tail<b.Tail;
        }else if(a.Rights.size()!=b.Rights.size()){
            return a.Rights.size()<b.Rights.size();
        }else{
            for(int i=0;i<a.Rights.size();i++){
                if(a.Rights[i]!=b.Rights[i]){
                    return a.Rights[i]<b.Rights[i];
                }
            }
            return true;
        }
    }
};
struct Closure{             //项目集
    set<Item>items;
    int state;
    //运算符重载
    friend bool operator ==(Closure a,Closure b){
       
        return a.items==b.items;
        
    }
    friend bool operator <(Closure a,Closure b){
        return a.state<b.state;
    }
};
struct Relation{//记录下两个集合之间转换的关系state1->state2:vector<string>
    set<string>v;
};
struct Table_Item{//LR(1)分析表
    int STATE;
    map<string,int>ACTION;
    map<string,int>GOTO;
};
struct Table{
    vector<Table_Item>tabel_item;//每一个条目
};

/*定义全局变量*/
vector<Grammar>grammars;//存储从文件读取的二型文法
set<string>Vn;//非终结符
set<string>Vt;//终结符
map<string,set<string> >first_set;//first集合
map<string,bool>toNull;//能推出空的语法左部
set<Closure>closures;//项目集合
vector<vector<int> >rels(200);//主要是降低时间复杂度，存储两个状态之间是否有关系
Relation relations[300][300];
vector<Token>tokens;
Table table;//全局变量：LR(1)分析表
//语义分析
vector<SignTable>signtable;//全局符号表
/*定义全局函数*/
//读取二型文法
Grammar getLineGrammar(string str){
    int len=str.length();
    Grammar ans;
    bool hasLeft=false;
    bool flag=false;
    string temp="";
    for(int i=0;i<len;i++){
        if(str[i]=='['){
            flag=true;
        }else if(str[i]==']'){
            flag=false;
            if(temp!=""){
                if(!hasLeft){
                    ans.Left=temp;
                    Vn.insert(temp);
                    hasLeft=true;
                }else{
                    if(!(temp[0]<='Z'&&temp[0]>='A')){
                        Vt.insert(temp);
                    }
                    ans.Rights.push_back(temp);
                }
            }
            temp="";
        }else{
            if(flag){
                temp+=str[i];
            }
        }
    }
    if(ans.Rights.size()==1&&ans.Rights[0]=="final"){
        toNull[ans.Left]=true;
    }
    return ans;
}
bool isVT(string str){
    if(str[0]<='Z'&&str[0]>='A'){
        return false;
    }else{
        return true;
    }
}
bool isVn(string str){
    if(str.length()!=0){
        if(str[0]<='Z'&&str[0]>='A'){
            return true;
        }
    }
    return false;
}

//1、求非终结符的First集合

//求非终结符的First集合
bool getFirst_init(string vn){
    set<string>ans;
    set<string>nu;
    nu.insert("final");
    bool flag=true;
    //最简单的那种就是遍历完原始公式，直接得出First集合的
    for(int i=0;i<grammars.size();i++){
       if(grammars[i].Left==vn){
           string f=grammars[i].Rights[0];
           int len=grammars[i].Rights.size();
           int cnt=0;
           while(cnt<len){
               if(isVT(f)){
                    ans.insert(f);
                    break;
                }else if(first_set.find(f)!=first_set.end()){
               //需要判断该非终结符是否可以推导出final空
                    if(toNull.find(f)==toNull.end()){
                    //推导出空，需要继续下一个的判断
                    set<string>temp;//求与final的差集
                    set_difference( first_set[f].begin(), first_set[f].end(),nu.begin(), nu.end(),inserter( temp, temp.begin() ) );    /*取差集运算*/
                    ans.insert(temp.begin(),temp.end());
                    cnt++;
                    }else{
                    //没有推导出空，则该终结符的First集合合并到当前
                        ans.insert(first_set[f].begin(),first_set[f].end());
                        break;
                    }
                }else{
                    flag=false;//有没有得到过的First集合
                    break;
                }
           }
           if(cnt>=len){
               //说明遍历到了尾巴
               ans.insert("final");
           }
       }
    }
    if(flag){
        first_set[vn]=ans;
    }
    return flag;
    
}
void _init_(){
    ifstream input;
    input.open(grammarfile);
    if(!input.is_open()){
        printf("FAILED TO LOAD GRAMMARS\n");
    }else{
        while(input.getline(buffer,maxn-1)){
            grammars.push_back(getLineGrammar(buffer));
        }
    }
    //求First集合
    int cnt=1;
    while(cnt){
        cnt=0;
        for(set<string>::iterator it=Vn.begin();it!=Vn.end();it++){
            if(getFirst_init(*it)==false){
                cnt++;
            }
        }
    }
}
//2、LR项目集的构造
//得到“·”的下一个字符
string getNext(Item item){
    int next_pos=item.pos;
    if(next_pos>=item.Rights.size()){
        return "[DANGER]";//形如：A->BC· 没有下一个了
    }else if(item.Rights[next_pos]=="final"){
        return "[FINAL]";//形如：A->·final 没有下一个字符
    }else{
        return item.Rights[next_pos];
    }
}
//构造转换函数
Closure Go(Closure clo,string str){
    //closure-str->new_closure
    Closure ans;
    Item temp_item;
    //原来的clo，对于下一位是str的移动后的全体
    for(set<Item>::iterator it=clo.items.begin();it!=clo.items.end();it++){
        if(getNext(*it)==str){
            temp_item.Left=(*it).Left;
            temp_item.Rights=(*it).Rights;
            temp_item.pos=(*it).pos+1;
            temp_item.Tail=(*it).Tail;
            ans.items.insert(temp_item);
        }
    }

   return ans;
}

//得到A->·Ban,op中的anop
vector<string> getPreTail(Item item){
    vector<string>ans;
    for(int i=item.pos+1;i<item.Rights.size();i++){
        ans.push_back(item.Rights[i]);
    }
    //接下来是原本的尾巴
    ans.push_back(item.Tail);
    return ans;
}
//得到First(ba)
set<string> getFirst(vector<string>ba){
    set<string>ans;
    for(int i=0;i<ba.size();i++){
        if(isVT(ba[i])){
            ans.insert(ba[i]);
            break;
        }else if(toNull.find(ba[i])!=toNull.end()){
            //当下可以为空
            ans.insert(first_set[ba[i]].begin(),first_set[ba[i]].end());
        }else{
            ans.insert(first_set[ba[i]].begin(),first_set[ba[i]].end());
            break;
        }
    }
    return ans;
}
//构造闭包函数
void getClosure(Closure &clo){
    //A->a·Bb,c
    bool flag;
    while(1){
        flag=false;
        for(set<Item>::iterator it=clo.items.begin();it!=clo.items.end();it++){//遍历项目集合
            string str=getNext(*it);
            Grammar temp_grammar;
            if(str=="[DANGER]"||str=="[FINAL]"){
                continue;
            }else if(isVn(str)){
                //寻找B->这样
                //找到相应的Tails
                for(int i=0;i<grammars.size();i++){
                    if(grammars[i].Left==str){
                        temp_grammar=grammars[i];
                        //找到grammar对应的Tails
                        vector<string>ba=getPreTail(*it);//首先找到ba
                        set<string>b=getFirst(ba);
                        //构造新的items
                        Item new_item;
                        new_item.Left=temp_grammar.Left;
                        new_item.Rights=temp_grammar.Rights;
                        new_item.pos=0;
                        for(set<string>::iterator sit=b.begin();sit!=b.end();sit++){
                            new_item.Tail=*sit;
                            //看看项目集中是否包含生成的新的项目
                            if(find(clo.items.begin(),clo.items.end(),new_item)==clo.items.end()){
                                //如果不包含
                                flag=true;
                                clo.items.insert(new_item);
                            }
                    }
                }else{
                    continue;
                }
            }
        }
        }
        //闭包不再增大
        if(flag==false){
            break;
        }
    }
}
void build_LR(){
    //构造LR(1)项目集闭包函数
    Item item;
    item.Left=grammars[0].Left;
    item.Rights=grammars[0].Rights;
    item.pos=0;
    item.Tail="final";
    Closure clo;
    clo.items.insert(item);
    clo.state=0;
    getClosure(clo);//对于第一项进行闭包
    closures.insert(clo);
    stack<Closure>st;//存储没有GO过的closure
    st.push(clo);
    Closure temp_clo;//生成的
    int cnt=0;//记录项目集编号
    while(!st.empty()){
        clo=st.top();
        st.pop();
        int nid=clo.state;
        int pid;
        //首先遍历得到所有可能的下一个str
        set<string>strs;
        for(set<Item>::iterator it=clo.items.begin();it!=clo.items.end();it++){
            string nx=getNext(*it);
            if(nx!="[FINAL]"&&nx!="[DANGER]"){
                strs.insert(nx);
            }
        }
        //Go,新建一个项目
        printf("***********************next str:%d\n",strs.size());
        for(set<string>::iterator it=strs.begin();it!=strs.end();it++){
            temp_clo=Go(clo,*it);
            getClosure(temp_clo);
            if(temp_clo.items.size()==0){
                //空项目，下一个
                continue;
            }
            //判断这个项目之前是否有过
            set<Closure>::iterator cit=find(closures.begin(),closures.end(),temp_clo);
            if(cit!=closures.end()){
                //之前有过
                pid=(*cit).state;
            }else{
                //之前没有过
                cnt++;
                pid=cnt;
                temp_clo.state=cnt;
                closures.insert(temp_clo);
                st.push(temp_clo);
            }
            //记录一下
            printf("%d %d\n",pid,nid);
            rels[nid].push_back(pid);
            relations[nid][pid].v.insert(*it);
        }
        printf("ok\n");
        
    }
}
//如果当前pos指向位置的下一个是r,获取r在原来产生式的编号
int getRid(string Left,vector<string>Rights){
    for(int i=0;i<grammars.size();i++){
        if(grammars[i].Left==Left){
            if(Rights.size()==grammars[i].Rights.size()){
                bool flag=true;
                for(int i=0;i>Rights.size();i++){
                    if(Rights[i]!=grammars[i].Rights[i]){
                        flag=false;
                        break;
                    }
                }
                if(flag){
                    return i;
                }
            }
        }
    }
    return -1;
}
//构建LR分析表
void build_Table(){
    //还需要存，就是遍历到最后的那种
    for(set<Closure>::iterator it=closures.begin();it!=closures.end();it++){
        Table_Item titem;
        //Ii->Ij（中间隔着的是符号str）
        int nid=(*it).state;
        titem.STATE=nid;
        for(int i=0;i<rels[nid].size();i++){
            int j=rels[nid][i];
            for(set<string>::iterator it=relations[nid][j].v.begin();it!=relations[nid][j].v.end();it++){
                string str=*it;
                if(isVT(str)){
                    titem.ACTION[str]=j;
                }else{
                    titem.GOTO[str]=j;
                }
            }
        }
        //该项目集中有r:用什么标志捏：-1000+r:因为0不识别负数
        for(set<Item>::iterator tit=(*it).items.begin();tit!=(*it).items.end();tit++){
            if((*tit).pos==(*tit).Rights.size()){//A->a·
                int rid=getRid((*tit).Left,(*tit).Rights);
                titem.ACTION[(*tit).Tail]=rsign+rid;
            }
        }
        table.tabel_item.push_back(titem);
    }
}
//LR读入词法分析结果，输出对于程序语法是否合法的判断
//读取词法分析器的处理结果
Token getToken(string str){
    Token token;
    stringstream sstream(str);
    int cnt=0;
    while(sstream){
        string substr;
        sstream>>substr;
        cnt++;
        if(cnt==1){
            token.line=stoi(substr);
        }
        if(cnt==2){
            token.word=substr;
        }else if(cnt==3){
            //0-界符 1-关键词 2-运算符 3-常量 4-标识符 
            if(substr=="keyword"){
                token.type=1;
            }else if(substr=="sign"){
                token.type=4;
            }else if(substr=="limited"){
                token.type=0;
            }else if(substr=="const"){
                token.type=3;
            }else if(substr=="operator"){
                token.type=2;
            }else{
                token.type=-1;//非法
            }
        }
    }
    return token;
}
void readWord_res(){
    ifstream input;
    input.open(tokenfile);
    if(!input.is_open()){
        printf("FAILED TO LOAD TOKENS\n");
    }else{
        while(input.getline(buffer,maxn-1)){
            tokens.push_back(getToken(buffer));
        }
    }
}
//输入当前状态以及输入的字符串
int getStateId(int cur_state_id,string str){
    //TODO:这里要确定一下是否可以直接用索引
    for(int i=0;i<table.tabel_item.size();i++){
        if(table.tabel_item[i].STATE==cur_state_id){
            if(table.tabel_item[i].ACTION.find(str)!=table.tabel_item[i].ACTION.end()){
                return table.tabel_item[i].ACTION[str];
            }else{
                printf("%d Failed to find ",cur_state_id);
                cout<<str<<"!\n";
                return failed;
            }
            
        }
    }
    return failed;
    
}
int getGotoId(int state_id,string str){
    //TODO:这里要确定一下是否可以直接用索引
    for(int i=0;i<table.tabel_item.size();i++){
        if(table.tabel_item[i].STATE==state_id){
            if(table.tabel_item[i].GOTO.find(str)!=table.tabel_item[i].GOTO.end()){
                return table.tabel_item[i].GOTO[str];
            }else{
                return failed;
            }
            break;
        }
    }
    return failed;
}
//LR(1)分析
/*
注意：
0-界符 
1-关键词 
2-运算符 
3-常量 
4-标识符 
*/
void ERR(int cur_state_id,string str){
    //当前的状态下标，以及下一个下标，原因
    //TODO:这里要确定一下是否可以直接用索引
    for(int i=0;i<table.tabel_item.size();i++){
        if(table.tabel_item[i].STATE==cur_state_id){
            //遍历当前状态可能的ACTION
            for(map<string,int>::iterator it=table.tabel_item[i].ACTION.begin();it!=table.tabel_item[i].ACTION.end();it++){
                cout  << "\033[31m"<<it->first<<"\t"<<"\033[0m"<< endl ;
            }
            
        }
    }
}
bool Analyse_LR(vector<Token>tokens){
    stack<int>states;//状态栈
    stack<string>symbols;//符号栈
    stack<Token>strs;//数值栈(原本代码中的部分)
    states.push(0);//初始状态都为0
    string str;//当前输入的字符串
    bool needpush=true;
    bool has_err=false;
    for(int i=0;i<tokens.size();i++){
        //TODO:输出检测 cout<<"当前输入token:"<<tokens[i].word<<endl;
        if(needpush){
            strs.push(tokens[i]);
        }
        //首先符号换一种方式表达
        if(tokens[i].type==0){
            //如果是界符，需要具体的
            str="\'";
            str+=tokens[i].word;
            str+="\'";
        }else if(tokens[i].type==1){
            //如果是关键字，也需要具体的
            str="\'";
            str+=tokens[i].word;
            str+="\'";
        }else if(tokens[i].type==2){
            //如果是运算符，需要具体的
            str="\'";
            str+=tokens[i].word;
            str+="\'";
        }else if(tokens[i].type==3){
            //如果是常量或者标识符，也不需要具体的
            str="const";
        }else if(tokens[i].type==4){
            str="sign";
        }
        //在Table中找到当前状态对应的下一个ACtION
        int cur_state_id=states.top();
        int next_state_id=getStateId(cur_state_id,str);
        //cout<<"cur_str\t\t"<<str<<" "<<cur_state_id<<" "<<next_state_id<<"\n\n";
        if(next_state_id==failed){
            printf("\033[31mFAILED!\t\tline:%d expect input:\033[0m",tokens[i].line);
            ERR(cur_state_id,str);
            printf("\n");
            return false;
        }else if(next_state_id<0){
            //这里的意思是，r,获取r的表示
            next_state_id-=rsign;
            //获取r对应的产生式
            Grammar grammar=grammars[next_state_id];
            //TODO：R,生成四元式,依据不同的r值去进行相应的操作
            cout<<"当前字符串："<<str<<" 用到的推理的："<<next_state_id<<"|"<<grammar.Left<<"->"<<grammar.Rights[0]<<"```\n";
            Quaternary temp=meaning_process(next_state_id,strs,signtable,has_err);
            if(temp.op!=""){
                quads.push_back(temp);
            }
            
            //状态弹栈、符号栈弹栈
            for(int i=grammar.Rights.size()-1;i>=0;i--){
                symbols.pop();
                states.pop();
            }
            //新的符号压栈
            symbols.push(grammar.Left);
            //然后是状态用GOTO转移
            int goto_id=getGotoId(states.top(),grammar.Left);
            states.push(goto_id);
            i--;//就是，输入串不变
            needpush=false;
        }else{
            //普通的S跳转
            states.push(next_state_id);
            symbols.push(str);
            needpush=true;
        }
    }
    return true;
}
//测试LR(1)分析是否正确
void Test(){
    if(Analyse_LR(tokens)){
        printf("YES\n");
    }else{
        printf("NO\n");
    }
}
/*一些找bug的函数，记得删除*/
void show_init(){
    //展现初始化结果是否正确
    printf("Grammar:\n");
    for(int i=0;i<grammars.size();i++){
        cout<<grammars[i].Left<<" ";
        for(int j=0;j<grammars[i].Rights.size();j++){
            cout<<grammars[i].Rights[j]<<",";
        }
        cout<<"\n";
    }
    printf("Vn:\n");
    for(set<string>::iterator it=Vn.begin();it!=Vn.end();it++){
        cout<<*it<<endl;
    }
    printf("Vt:\n");
    for(set<string>::iterator it=Vt.begin();it!=Vt.end();it++){
        cout<<*it<<endl;
    }
    printf("First:\n");
    for(map<string,set<string> >::iterator it=first_set.begin();it!=first_set.end();it++){
        cout<<it->first<<":";
        for(set<string>::iterator mit=(it->second).begin();mit!=(it->second).end();mit++){
            cout<<*mit<<" ";
        }
        cout<<"\n";
    }
    //展示token表
    printf("Tokens:\n");
    for(int i=0;i<tokens.size();i++){
        cout<<tokens[i].type<<" : "<<tokens[i].word<<"\n";
    }
}
//展示一下LR项目集
void showLR(){
    printf("***************************************%d\n",closures.size());
    for(set<Closure>::iterator it=closures.begin();it!=closures.end();it++){
        //展现closure
        printf("%d:\n",(*it).state);
        //遍历项目集
        for(set<Item>::iterator iit=(*it).items.begin();iit!=(*it).items.end();iit++){
            cout<<(*iit).Left<<" : ";
            for(int i=0;i<(*iit).Rights.size();i++){
                if(i){
                    printf(",");
                }
                cout<<(*iit).Rights[i];
            }
            cout<<"\t\t\t|"<<(*iit).Tail<<endl;
        }
    }
}
void showTable(){
    //展现Table
    for(int i=0;i<table.tabel_item.size();i++){
        printf("-----------------------------------------------------------\n");
        printf("%d:\n",table.tabel_item[i].STATE);
        printf("ACTION:\n");
        for(map<string,int>::iterator it=table.tabel_item[i].ACTION.begin();it!=table.tabel_item[i].ACTION.end();it++){
            cout<<"["<<it->first<<"]:"<<it->second<<endl;
        }
        printf("GOTO:\n");
        for(map<string,int>::iterator it=table.tabel_item[i].GOTO.begin();it!=table.tabel_item[i].GOTO.end();it++){
            cout<<"["<<it->first<<"]:"<<it->second<<endl;
        }
    }
}
void showMeaning(){
//展示四元组生成结果
    for(int i=0;i<quads.size();i++){
        cout<<"("<<quads[i].op<<","<<quads[i].result<<","<<quads[i].arg1<<","<<quads[i].arg2<<")"<<endl;
    }
}
int main(){
    _init_();
    readWord_res();
    //show_init();
    
    build_LR();
    printf("-----------------------------------show---------------------------------------\n");
    // showLR();
    build_Table();
    // showTable();
    //测试是否正确
    Test();
    printf("-----------------------------------show---------------------------------------\n");
    //展示四元组
    showMeaning();


    return 0;  
}