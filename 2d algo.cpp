
#include <iostream>
#include <fstream>
#include <string>
#include<map>
#include<stack>
#include<vector>
#include <chrono>
using namespace std;
using namespace std::chrono;
struct V {
    int variable;
    int value;
    int abs_value = -1;
    V() :variable(-1), value(-1) {}
    V(int v) :variable(v), value(-1) {}
    bool  set_value(bool a)
    {
        if (abs_value != -1)
        {
            value = abs_value;
                return false;
        }
        
        value = (a) ?1 : 0;
        return true;
    }
    bool is_empty()
    {
        return (value == -1);
    }
    bool is_fixedValue()
    {
        return(abs_value != -1);
    }
    bool Val()
    {
        if (value == 0)
        {
            return false;
        }
        else if (value == 1)
        {
            return true;
        }
        else
        {
            cout << "problem occured\n";
            return false;
        }
    }
    bool operator< (const V& b) const
    {
        return variable < b.variable;
    }
};
struct node{
   struct V* v;
    bool isPos;
    bool isvisited = false;
    vector<node*> front ;
    vector<node*> back;
    node() : v(NULL), isPos(NULL) {}
    node( V* var) : v(var), isPos(NULL) {}
    node( V* var , bool v) : v(var), isPos(v) {}
    void add_front(node* node) 
    {
        front.push_back(node);
    }
    void add_reverse(node* node)
    {
        back.push_back(node);
    }
    bool wasVisited()
    {
        return isvisited;
    }
    void visited()
    {
        isvisited = true;
    }
    void reset_visited()
    {
        isvisited = false;
    }
    bool SetTrue()
    {
       return v->set_value(isPos);
    }
    bool Value()
    {
        return (isPos) ? v->Val() : !v->Val();
    }

};
map<int,V> variable = map<int,V>();
map<int, node> graph;
stack<node*> prio_queue;
vector<map<int,node*>> scc;
stack<node*> gud;
void dfs(node * node)
{
    if (node->wasVisited())
    {
        return;
    }
    gud.push(node);
    while (!gud.empty())
    {
        if (gud.top()->wasVisited())
        {
            prio_queue.push(gud.top());
            gud.pop();
            continue;
        }
        gud.top()->visited();
        for (auto it : gud.top()->front)
        {
            if (!it->wasVisited())
            {
                gud.push(it);
            }
        }
    }
}

void dfs2(node* n)
{
    gud.push(n);
    while (!gud.empty())
    {
        node* neet = gud.top();
        gud.pop();
        neet->visited();
        int key = (neet->isPos) ? neet->v->variable : -neet->v->variable;
        scc.back().insert(pair<int, node*>(key, neet));
        for (auto it : neet->back)
        {
            if (!it->wasVisited())
            {
                gud.push(it);
            }
        }
    }
}
bool kosaraju()
{
    for (auto it = graph.begin();it != graph.end();it++)
    {
        if (!it->second.wasVisited())
        {
            dfs(&it->second);
        }
    }
  
    for (auto it = graph.begin();it != graph.end();it++)
    {
        it->second.reset_visited();
    }
    gud = stack<node*>();
    while (!prio_queue.empty())
    {
        node* Node = prio_queue.top();
        prio_queue.pop();
        if (!Node->wasVisited())
        {
            scc.push_back(map<int, node*>());
            dfs2(Node);
            for (auto it = scc.back().begin();it != scc.back().end();it++)
            {
                if (scc.back().find(-it->first) != scc.back().end())
                {

                    cout << "Formula is Unsatisfiable at " << it->first << "! program end \n";

                    return false;
                }
            }
            
        }
        
    }
    return true;
}

void edge_insert(V v)
{
    if (v.is_fixedValue())
    {
        int x = v.variable;
        node* pos = &graph.find(x)->second;
        node* neg = &graph.find(-x)->second;
        if (v.abs_value == 0)
        {
           pos->add_front(neg);
           neg->add_reverse(pos);
        }
        else {
            neg->add_front(pos);
            pos->add_reverse(neg);
        }
    }
}

void graph_insert(int f_v , pair<map<int, V>::iterator, bool> fp)
{
    int temp = abs(f_v);
    V* v = &(fp.first->second);
    node n_true = node(v, true);
    node n_false = node(v, false);
    graph.insert(pair<int, node>(temp, n_true));
    graph.insert(pair<int, node>(-temp, n_false));
}
int main()
{
    string line;
    ifstream file("largeUnsat.cnf");
    bool state = false;
    auto start = high_resolution_clock::now();
    while (getline(file,line))
    {
        if (!line.empty())
        {
            if (line.back() == '0' && state)
            {
                pair<map<int,V>::iterator, bool> fp;
                pair<map<int, V>::iterator, bool> sp;
                line = line.substr(0, line.size() - 2);
                size_t  pos = line.find(" ");
                string f = line.substr(0, pos);
                string sec = line.substr(pos + 1);
                int f_v = stoi(f);
                int f_sec = stoi(sec);
                if (f_v != f_sec)
                {
                    fp = variable.insert(pair<int, V>(abs(f_v), V(abs(f_v))));
                    sp = variable.insert(pair<int, V>(abs(f_sec), V(abs(f_sec))));
                    if (fp.second)
                    {
                        graph_insert(f_v, fp);
                    }
                    if (sp.second)
                    {
                        graph_insert(f_sec, sp);
                    }
                    /*cout << graph.find(f_sec)->second;*/

                    graph.find(-f_v)->second.add_front(&(graph.find(f_sec)->second));
                    graph.find(-f_sec)->second.add_front(&(graph.find(f_v)->second));
                    graph.find(f_v)->second.add_reverse(&(graph.find(-f_sec)->second));
                    graph.find(f_sec)->second.add_reverse(&(graph.find(-f_v)->second));
                    edge_insert(fp.first->second);
                    edge_insert(sp.first->second);

                }

                else {
                    fp = variable.insert(pair<int, V>(abs(f_v), V(abs(f_v))));
                    bool temp= (f_v < 0) ? false : true;
                    fp.first->second.abs_value = temp;
                    if (!fp.second)
                    {
                        edge_insert(fp.first->second);
                    }
                    }
               
            }
            if (line.front() == 'p')
            {
                state = true;
            }
        }
       
    }
    if (variable.empty() || graph.empty())
    {
        cout << "error occured ! variable is empty !" << "\n";
           return 0;
    }
    cout << "start\n";
    bool a = kosaraju();
    if (!a)
    {
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout<<" time Used : "<<duration.count()<<"ms\n";
        return 0;
    }
    else
    {
        vector<node*> check_value;
        for (auto it = scc.rbegin() ;it != scc.rend() ;++it)
        {   
           
            for (auto n = it->begin();n != it->end();n++)
            {
                if (n->second->v->is_empty())
                {
                   bool a= n->second->SetTrue();
                   
                   if (!a)
                   {
                       check_value.push_back(n->second);
            
                   }
                }
               
            }
        }

    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Formula is Satisfiable \n time Used:"<<duration.count()<<"ms\n";
    ofstream MyFile("BoolAssignment.txt");
    for (auto it = variable.begin();it != variable.end();it++)
    {
        string boolean = (it->second.value) ? "TRUE" : "FALSE";
        MyFile << it->second.variable << ":" + boolean + "\n";
        cout << it->second.value << " ";
    }
    return 0;
}
