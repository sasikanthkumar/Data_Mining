#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <algorithm>
#include <typeinfo>

#include <iterator>
#include <vector>

using namespace std;

float min_support = 0.3;
float no_of_trans = 0;

ifstream input_file;

set<string> items;

string input_file_name = "groceries_temp.csv";


set<set<int> > transactions_int;



bool check_subsets(set<int> can_ans, set<set<int> > fk_1){
    
    set<int>::iterator stop_it = can_ans.end();
    stop_it--;
    stop_it--;
    
    for (set<int>::iterator it = can_ans.begin(); it != stop_it; it++) {
        set<int> temp_set = can_ans;
        temp_set.erase(*it);
        set<set<int> >::iterator it1 = fk_1.find(temp_set);
        if (it1 == fk_1.end()) {
            return false;
        }
    }
    return true;
}

set<set<int> > apriori_gen(set<set<int> > fk_1){
    
    set<set<int> > ck;
    
    
    for (set<set<int> >::iterator it1 = fk_1.begin(); it1 != fk_1.end(); it1++) {
        set<set<int> >::iterator it2 = it1;
        for (it2++; it2 != fk_1.end(); it2++) {
            set<int> first = *it1;
            set<int> second = *it2;
            set<int>::iterator it_first = first.end();
            set<int>::iterator it_second = second.end();
            --it_first;
            --it_second;
            
            int last_second = *it_second;
            int last_first = *it_first;
            
            first.erase(it_first);
            second.erase(it_second);
            
            if (first == second) {
                set<int> can_ans = *it1;
                can_ans.insert(last_second);
                if(check_subsets(can_ans,fk_1)){
                    ck.insert(can_ans);
                }
            }
        }
    }
    
    return ck;

}


map<set<int>,int> leaf_elem_supp;  // support counts for ck
map <string, int> item_ids_map;
map <int, string> ids_item_map;

struct node{
    struct node * parent;
    bool leaf;  // if leaf node true
    int curr_hash_int;  // level
    int elem_count;  // no of itemsets stored in node
    set<set<int> > node_elems; // candidate itemsets
    vector<struct node *> child_ptr;  // branches pointers
    
};

struct node * newnode(int k){  // k for no of branches and mod k same as max element count
    struct node * temp_node = (struct node *)malloc(sizeof(struct node));  // creating new node
    temp_node->parent = NULL;
    temp_node->curr_hash_int = 0;  // current level
    temp_node->leaf = true;
    temp_node->elem_count =0;  // no of items stored
    cout << "pp" << endl;
    cout << (temp_node->node_elems).size() << endl;
    cout << (temp_node->node_elems).empty() << endl;
    
    (temp_node->node_elems).erase((temp_node->node_elems).begin(),(temp_node->node_elems).end());
    cout << "pp" << endl;

    cout << (temp_node->node_elems).size() << endl;
    temp_node->child_ptr.reserve(k);  // makes k branches having null
    cout << "pp" << endl;

    return temp_node;
}

struct node * root = NULL;


void insert(set<int> t, int max_elem_count){
    if(root == NULL){
        cout << "nnn" << endl;

        root = newnode(max_elem_count);
        cout << "nnn" << endl;

    }
    struct node * temp_node = root;
    struct node * trav_node = root;

    while (temp_node->leaf != true) {
        // now we should find hash value
        int current_level = temp_node->curr_hash_int;
        int hash_val = 0;
        set<int>::iterator temp_it = t.begin();
        advance(temp_it, current_level);
        hash_val = (*temp_it)% max_elem_count;
        temp_node = temp_node->child_ptr[hash_val];
    }
    

    if (temp_node->leaf == true) {
        if (temp_node->elem_count < max_elem_count) {// no need to split
            cout << ((temp_node->node_elems)).size() << endl;
            (temp_node->node_elems).insert(t);

            (temp_node->elem_count)++;
        }else{
            (temp_node->elem_count)++;
            temp_node->leaf = false;
            int j=temp_node->curr_hash_int;  // level for hashing on which element
            // branches initialization
            for (int i = 0; i < max_elem_count; ++i)
            {
                temp_node->child_ptr[i] = newnode(max_elem_count);
                (temp_node->child_ptr[i])->parent = temp_node;
                temp_node->child_ptr[i]->curr_hash_int = (temp_node->curr_hash_int) + 1;
            }
            // apply hash function to every itemset already stored in node
            for(set<set<int> >::iterator it= (temp_node->node_elems).begin();it != (temp_node->node_elems).end();it++){
                int val = 0;
                set<int> temp_set = *it;
                set<int>::iterator it1=temp_set.begin();
                advance(it1,j);
                
                val = *it1;
                val = val%max_elem_count;
                ((temp_node->child_ptr[val])->node_elems).insert(temp_set);
                (temp_node->child_ptr[val])->elem_count++;
            }
            
            // insert new candidate element
            int hash_val = 0;
            set<int>::iterator temp_it = t.begin();
            advance(temp_it,j);
            hash_val = (*temp_it)%max_elem_count;
            ((temp_node->child_ptr[hash_val])->node_elems).insert(t);
            (temp_node->child_ptr[hash_val])->elem_count++;
        }
    }
}

set<set<int> > subsets_of_size_k;
void generate_subsets_of_size_k(set<int> A, int k, int start, int currLen, bool used[]){
    if (currLen == k) {
        int i = 0;
        set<int> temp_set;
        for (set<int>::iterator it = A.begin(); it != A.end(); it++) {
            if (used[i] == true) {
                temp_set.insert(*it);
            }
            i++;
        }
        subsets_of_size_k.insert(temp_set);
        return;
    }
    if (start == A.size()) {
        return;
    }
    used[start] = true;
    generate_subsets_of_size_k(A,k,start+1,currLen+1,used);
    used[start] = false;
    generate_subsets_of_size_k(A,k,start+1,currLen,used);
    
}


void pass_through_hashtree(set<int> t){
    struct node * temp_node = root;
    struct node * trav_node = root;
    
    
    while (temp_node->leaf != true) {
        // now we should find hash value
        int current_level = temp_node->curr_hash_int;
        int hash_val = 0;
        set<int>::iterator temp_it = t.begin();
        advance(temp_it, current_level);
        hash_val = (*temp_it)% (t.size());
        temp_node = temp_node->child_ptr[hash_val];
    }
    
    set<set<int> > elems_in_temp_node = temp_node->node_elems;
    set<set<int> >::iterator it = elems_in_temp_node.find(t);
    if (it != elems_in_temp_node.end()) {
        (leaf_elem_supp.find(t))->second++;
    }

}

set<set<int> > generate_fk_frm_ck(set<set<int> > ck){
    // construct hash tree
    set<set<int> >::iterator it = ck.begin();
    set<set<int> > fk;
    
    int max_elem_count = (*it).size();
    
    root = NULL;
    
    leaf_elem_supp.clear();
    
    cout << "bb" << endl;
    for (set<set<int> >::iterator it1 = ck.begin(); it1 != ck.end(); it1++) {
        insert(*it1,max_elem_count);
        cout << "sss" << endl;

        leaf_elem_supp.insert(pair<set<int> , int>(*it1,0));
    }
    // pass transactions and count support
    for (set<set<int> >::iterator it = transactions_int.begin() ; it != transactions_int.end(); it++) {
        subsets_of_size_k.clear();
        set<int> temp_set = *it;
        if (temp_set.size() >= max_elem_count) {
            bool b[temp_set.size()];
            for (int i = 0; i<temp_set.size(); i++) {
                b[i] = false;
            }
            generate_subsets_of_size_k(temp_set,max_elem_count,0,0,b);
            for (set<set<int> >::iterator it2 = subsets_of_size_k.begin(); it2 != subsets_of_size_k.end(); it2++) {
                pass_through_hashtree(*it2);
            }
        }
    }
    // passing transasctions finished
    
    // iterate througn leaf elemnets support
    for (map<set<int>, int>::iterator it = leaf_elem_supp.begin(); it != leaf_elem_supp.end(); it++) {
        float itemset_support = ((*it).second)/no_of_trans;
        if (itemset_support > min_support) {
            fk.insert((*it).first);
        }
    }
    
    // return freqitems sets
    return fk;
    
}

int main(){
    
    input_file.open( input_file_name );
    
    while (input_file){
        string s;
        if (!getline( input_file, s )) break;
        no_of_trans++;
        istringstream ss( s );
        while (ss)
        {
            string s;
            if (!getline( ss, s, ',' )) break;
            items.insert(s);
        }
    }
    
    input_file.close();
    
    
    cout << items.size() << endl;

    int id = 1;
    for (set<string>::iterator it = items.begin(); it != items.end(); it++) {
        item_ids_map.insert(pair<string, int>(*it, id));
        ids_item_map.insert(pair<int, string>(id, *it));
        id++;
    }
    
    /**
    for (map<string, int>:: iterator it = item_ids_map.begin(); it != item_ids_map.end(); it++) {
        cout << it->first << " -- " << it ->second << endl;
    }
    **/
    
    vector<set<set<int> > > freq_itemsets;
    
    
    // finding 1 freq itemsets
    map<set<int>,int> temp_map;
    for (int i = 1; i<=items.size(); i++) {
        set<int> temp_set;
        temp_set.insert(i);
        temp_map.insert(pair<set<int>,int>(temp_set,0) );
    }
    
    /**
    for (map<set<int>,int>::iterator it = temp_map.begin(); it != temp_map.end(); it++) {
        set<int> temp_set = it->first;
        cout << "{";
        for (set<int>::iterator it1 = temp_set.begin(); it1 != temp_set.end(); it1++) {
            cout << *it1 << ",";
        }
        cout << "} -- " << it->second << endl;
    }
    **/
    
    // iterating through every trasaction for counting support
    
    input_file.open( input_file_name);
    while (input_file)
    {
        string s;
        if (!getline( input_file, s )) break;
        istringstream ss( s );
        
        set<int> trans_temp;
        while (ss)
        {
            string s;
            if (!getline( ss, s, ',' )) break;
            int item_id = item_ids_map.find(s)->second;
            set<int> temp_set;
            temp_set.insert(item_id);
            trans_temp.insert(item_id);
            temp_map.find(temp_set)->second++;
        }
        transactions_int.insert(trans_temp);
        
    }
    input_file.close();
    
    set<set<int> > f1;
    
    for (map<set<int>,int>::iterator it = temp_map.begin(); it != temp_map.end(); it++) {
        float itemset_support = (it->second)/no_of_trans;
        set<int> temp_set = it->first;
        if (itemset_support > min_support) {
            f1.insert(temp_set);
        }
    }
    
    freq_itemsets.push_back(f1);
    
    
    // priting freq 1 item sets
    /**
    for (set<set<int> >::iterator it = f1.begin(); it != f1.end(); it++) {
        set<int> temp_set  = *it;
        cout << "{";
        for (set<int>::iterator it1 = temp_set.begin(); it1 != temp_set.end(); it1++) {
            cout << ids_item_map.find(*it1)->second << "," ;
        }
        cout << "}" << endl;
    }
    
    **/
    
    int k = 1;
    
    while (true) {
        k = k+1;
        set<set<int> > fk;
        set<set<int> > ck = apriori_gen(freq_itemsets[k-2]);
        // printing candidate itemsets
        /**
        for (set<set<int> >::iterator it = ck.begin(); it != ck.end(); it++) {
            set<int> temp_set  = *it;
            cout << "{";
            for (set<int>::iterator it1 = temp_set.begin(); it1 != temp_set.end(); it1++) {
                cout << ids_item_map.find(*it1)->second << "," ;
            }
            cout << "}" << endl;
        }
        **/
        // printing candidate itemsets
        // support counting for candidate itemsets
        // hash tree constructions and passing transactions
        
        
        // break if no candidates are generated
        
        if (ck.size() == 0) {
            break;
        }
        cout << "bef gen" << endl;
        fk = generate_fk_frm_ck(ck);
        cout << "end gen" << endl;
        if (fk.size() == 0) {
            break;
        }else{
            freq_itemsets.push_back(fk);
        }
    }
    
    for (int i = 0; i<freq_itemsets.size(); i++) {
        cout << i+1 << " frequent itemsets" << endl;
        set<set<int> > temp_set_set = freq_itemsets[i];
        for (set<set<int> >::iterator it = temp_set_set.begin(); it != temp_set_set.end(); it++) {
            set<int> temp_set = (*it);
            cout << "{";
            for (set<int>::iterator it1 = temp_set.begin(); it1 != temp_set.end(); it1++) {
                cout << (*it1) << ",";
            }
            cout << "}" << endl;
        }
        cout << endl;
        
    }
    
    
    return 0;
}



