#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <algorithm>
#include <typeinfo>
#include <algorithm>

#include <iterator>
#include <vector>

using namespace std;

float min_support = 0.01;
float no_of_trans = 0;
float min_conf = 0.1;

ifstream input_file;

set<string> items;

string input_file_name = "groceries_temp.csv";

int node_count = 1;
set<set<int> > transactions_int;


struct node{
    int node_no;
    struct node * parent;
    bool leaf;  // if leaf node true
    int curr_hash_int;  // level
    int elem_count;  // no of itemsets stored in node
    //set<set<int> > node_elems; // candidate itemsets
    //vector<struct node *> child_ptr;  // branches pointers
    
};


map<int, set<set<int> > > nodeNo_itemsets_map;
map<int, vector<struct node *> >  nodeNo_child_ptrs_map;
map<set<int> , int> freq_itemsets_support_count_map;

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



struct node * newnode(int k){  // k for no of branches and mod k same as max element count
    struct node * temp_node = (struct node *)malloc(sizeof(struct node));  // creating new node
    temp_node->node_no = 1;
    temp_node->parent = NULL;
    temp_node->curr_hash_int = 0;  // current level
    temp_node->leaf = true;
    temp_node->elem_count =0;  // no of items stored
    //cout << (temp_node->node_elems).size() << endl;
    //cout << (temp_node->node_elems).empty() << endl;
    
    //(temp_node->node_elems).erase((temp_node->node_elems).begin(),(temp_node->node_elems).end());
    
    //cout << (temp_node->node_elems).size() << endl;
    //temp_node->child_ptr.reserve(k);  // makes k branches having null
    
    return temp_node;
}

struct node * root = NULL;


void insert(set<int> t, int max_elem_count){
    if(root == NULL){
        //cout << "nnn" << endl;
        
        root = newnode(max_elem_count);
        root->node_no = node_count;
        //cout << "node count = " << node_count << endl;
        node_count++;
        //cout << "nnn" << endl;
        
    }
    struct node * temp_node = root;
    struct node * trav_node = root;

    while (temp_node->leaf != true) {
        // now we should find hash value
        int current_level = temp_node->curr_hash_int;
        int hash_val = 0;
        set<int>::iterator temp_it = t.begin();
        
        if (current_level < t.size()) {
            advance(temp_it, current_level);
        }else{
            advance(temp_it,t.size()-1);
        }
        
        
        hash_val = (*temp_it)% max_elem_count;
        cout << "vvvv" << endl;

        // get child pointers for this node no
        map<int, vector<struct node *> >::iterator ptr_it_map = nodeNo_child_ptrs_map.find(temp_node->node_no);
        
        if (ptr_it_map == nodeNo_child_ptrs_map.end()) {  // not found error
            cout << "Error in finding child pointers" << endl;
        }else{ //found
            vector<struct node *> child_ptr_vector = (*ptr_it_map).second;
            temp_node = child_ptr_vector[hash_val];
        }
        
        
        //temp_node = temp_node->child_ptr[hash_val];
    }
    if (temp_node->leaf == true) {
        //cout << "temp_node->leaf == true" << endl;

        if (temp_node->elem_count < (max_elem_count*1000)) {// no need to split
            //cout << ((temp_node->node_elems)).size() << endl;
            // find in map;
            map<int, set<set<int> > >::iterator map_it = nodeNo_itemsets_map.find((temp_node->node_no));
            
            if (map_it == nodeNo_itemsets_map.end()) {  // not found
                set<set<int> > tt_set;
                tt_set.insert(t);
                pair<int, set<set<int> > > tt_pair((temp_node->node_no),tt_set);
                nodeNo_itemsets_map.insert(tt_pair);
            }else{  // found
                set<set<int> > already_pre = (*map_it).second;
                already_pre.insert(t);
                map_it->second = already_pre;
            }
            
            //(temp_node->node_elems).insert(t);
            
            (temp_node->elem_count)++;
        }else{

            (temp_node->elem_count)++;
            temp_node->leaf = false;
            int j=temp_node->curr_hash_int;  // level for hashing on which element
            // branches initialization
            vector<struct node *> to_store_vector;
            to_store_vector.clear();
            
            for (int i = 0; i < max_elem_count; ++i)
            {
                //cout << "temp_node->leaf == false" << endl;

                //temp_node->child_ptr[i] = newnode(max_elem_count);
                struct node * my_new_node = newnode(max_elem_count);
                
                //cout << "temp_node->leaf == false" << endl;
                my_new_node->node_no = node_count;
                
                //(temp_node->child_ptr[i])->node_no = node_count;
                cout << "node_count = " << node_count << endl;
                node_count++;
                my_new_node->parent = temp_node;
                //(temp_node->child_ptr[i])->parent = temp_node;
                //temp_node->child_ptr[i]->curr_hash_int = (temp_node->curr_hash_int) + 1;
                my_new_node->curr_hash_int = (temp_node->curr_hash_int) + 1;

                to_store_vector.push_back(my_new_node);

            }

            map<int, vector<struct node *> >::iterator jj_it_map = nodeNo_child_ptrs_map.find(temp_node->node_no);
            if (jj_it_map == nodeNo_child_ptrs_map.end()) {  // not initialized already so no error
                pair<int, vector<struct node *> > to_insert_pair_child_ptr(temp_node->node_no,to_store_vector);
                nodeNo_child_ptrs_map.insert(to_insert_pair_child_ptr);
            }else{  // found means error
                cout << "Error in inserting child branch pointers" << endl;
            }

            

            // find already pre itemsets in node
            map<int, set<set<int> > >::iterator map_it = nodeNo_itemsets_map.find((temp_node->node_no));
            
            set<set<int> > already_pre;

            if (map_it == nodeNo_itemsets_map.end()) {
                cout << "Error" << endl;
            }else{
                already_pre = (*map_it).second;
            }

            

            // apply hash function to every itemset already stored in node
            for(set<set<int> >::iterator it= already_pre.begin();it != already_pre.end();it++){
                int val = 0;
                set<int> temp_set = *it;
                set<int>::iterator it1=temp_set.begin();
                set<int>::iterator it3 = temp_set.end();
                cout << "node_c after for loop" << node_count << endl;
                cout << temp_set.size() << endl;
                cout << j << endl;
                if (j< temp_set.size()) {
                    advance(it1,j);
                }else{
                    advance(it1,temp_set.size()-1);
                }
                cout << "node_c after for loop" << node_count << endl;

                
                val = *it1;
                val = val%max_elem_count;
                // find if already present in map and append or insert
                vector<struct node*> child_pointers_vector;
                map<int, vector<struct node *> >::iterator bb_map_it = nodeNo_child_ptrs_map.find(temp_node->node_no);
                if (bb_map_it == nodeNo_child_ptrs_map.end()) {  // not found so error
                    cout << "Error" << endl;
                }else{
                    child_pointers_vector = (*bb_map_it).second;
                }

                int temp_node_num =(child_pointers_vector[val])->node_no;
                
                map<int, set<set<int> > >::iterator find_in_map_it = nodeNo_itemsets_map.find(temp_node_num);
                

                if (find_in_map_it == nodeNo_itemsets_map.end()) { // not found
                    set<set<int> > to_insert_set;
                    to_insert_set.insert(temp_set);
                    pair<int, set<set<int> > > to_insert_pair(temp_node_num,to_insert_set);
                    nodeNo_itemsets_map.insert(to_insert_pair);
                }else{  // found
                    set<set<int> > already_pre_set = (*find_in_map_it).second;
                    already_pre_set.insert(temp_set);
                    find_in_map_it->second = already_pre_set;
                }

                
                //((temp_node->child_ptr[val])->node_elems).insert(temp_set);
                
                
                
                
                // finding if already present and append or insert
                //(temp_node->child_ptr[val])->elem_count++;
                (child_pointers_vector[val])->elem_count++;
                

                
            }
            

            
            // insert new candidate element
            int hash_val = 0;
            set<int>::iterator temp_it = t.begin();
            
            if (j<t.size()) {
                advance(temp_it,j);
            }else{
                advance(temp_it,t.size()-1);
            }
            
            hash_val = (*temp_it)%max_elem_count;

            
            vector<struct node*> child_pointers_vector;
            map<int, vector<struct node *> >::iterator bb_map_it = nodeNo_child_ptrs_map.find(temp_node->node_no);
            if (bb_map_it == nodeNo_child_ptrs_map.end()) {  // not found so error
                cout << "Error" << endl;
            }else{
                child_pointers_vector = (*bb_map_it).second;
            }
            
            cout << "aaaa" << endl;

            // same task find and edit in map
            
            int gg_node_num =(child_pointers_vector[hash_val])->node_no;
            map<int, set<set<int> > >::iterator gg_map_it = nodeNo_itemsets_map.find(gg_node_num);
            
            if (gg_map_it == nodeNo_itemsets_map.end()) {  // not found
                set<set<int> > gg_to_insert;
                gg_to_insert.insert(t);
                pair<int, set<set<int> > > gg_to_insert_pair(gg_node_num,gg_to_insert);
                nodeNo_itemsets_map.insert(gg_to_insert_pair);
            }else{  // found
                set<set<int> > gg_already_pre_set = (*gg_map_it).second;
                gg_already_pre_set.insert(t);
                gg_map_it->second = gg_already_pre_set;
            }
            
            //((temp_node->child_ptr[hash_val])->node_elems).insert(t);
            
            
            
            // same task edit in map
            //(temp_node->child_ptr[hash_val])->elem_count++;
            
            (child_pointers_vector[hash_val])->elem_count++;
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
        
        if (current_level < t.size()) {
             advance(temp_it, current_level);
        }else{
            advance(temp_it,t.size()-1);
        }
        
       
        hash_val = (*temp_it)% (t.size());
        
        
        map<int, vector<struct node *> >::iterator ptr_it_map = nodeNo_child_ptrs_map.find(temp_node->node_no);
        
        if (ptr_it_map == nodeNo_child_ptrs_map.end()) {  // not found error
            cout << "Error in finding child pointers" << endl;
        }else{ //found
            vector<struct node *> child_ptr_vector = (*ptr_it_map).second;
            temp_node = child_ptr_vector[hash_val];
        }
        
        //temp_node = temp_node->child_ptr[hash_val];
    
        
    }
    
    //set<set<int> > elems_in_temp_node = temp_node->node_elems;
    set<set<int> > elems_in_temp_node;
    
    // find in map
    
    int node_number = temp_node->node_no;
    map<int, set<set<int> > >::iterator my_map_it = nodeNo_itemsets_map.find(node_number);
    
    if (my_map_it == nodeNo_itemsets_map.end()) {  // not found
        cout << "Error in passing transaction to tree" << endl;
    }else{  // found
        elems_in_temp_node = (*my_map_it).second;
        cout << "no of itemsets in leaf node"<< elems_in_temp_node.size() << endl;
        
        set<set<int> >::iterator it = elems_in_temp_node.find(t);
        if (it != elems_in_temp_node.end()) {
            (leaf_elem_supp.find(t))->second++;
        }
    }
}

set<set<int> > generate_fk_frm_ck(set<set<int> > ck){
    // construct hash tree
    set<set<int> >::iterator it = ck.begin();
    set<set<int> > fk;
    
    int max_elem_count = (*it).size();
    
    root = NULL;
    
    leaf_elem_supp.clear();
    
    //cout << "bb" << endl;
    cout << ck.size() << endl;
    
    for (set<set<int> >::iterator it2 = ck.begin(); it2 != ck.end(); it2++) {
        set<int> gggg = *it2;
        //cout << "sss" << endl;
        //cout << leaf_elem_supp.size() << endl;
        leaf_elem_supp.insert(pair<set<int> , int>(gggg,0));
    }
    cout << "After for loop" << endl;

    int index = 0;
    for (set<set<int> >::iterator it1 = ck.begin(); it1 != ck.end(); it1++) {
        cout << index << endl;
        
        insert(*it1,max_elem_count);
        cout << "After index inserting" << endl;

        index++;
        
    }
    cout << "mmm" << endl;
    int hhhh = 0;
    // pass transactions and count support
    for (set<set<int> >::iterator it = transactions_int.begin() ; it != transactions_int.end(); it++) {
        hhhh++;
        cout << "transaction number = " << hhhh << endl;
        subsets_of_size_k.clear();
        set<int> temp_set = *it;
        if (temp_set.size() >= max_elem_count) {
            bool b[temp_set.size()];
            for (int i = 0; i<temp_set.size(); i++) {
                b[i] = false;
            }
            generate_subsets_of_size_k(temp_set,max_elem_count,0,0,b);
            for (set<set<int> >::iterator it2 = subsets_of_size_k.begin(); it2 != subsets_of_size_k.end(); it2++) {
                cout << "transaction number = " << hhhh << endl;

                cout << "uu" << endl;
                pass_through_hashtree(*it2);
                cout << "ss" << endl;
            }
        }
    }
    
    cout << "yyy" << endl;
    // passing transasctions finished
    
    // iterate througn leaf elemnets support
    for (map<set<int>, int>::iterator it = leaf_elem_supp.begin(); it != leaf_elem_supp.end(); it++) {
        float itemset_support = ((*it).second)/no_of_trans;
        if (itemset_support > min_support) {
            fk.insert((*it).first);
            pair<set<int>, int> to_in_pair((*it).first,(*it).second);
            freq_itemsets_support_count_map.insert(to_in_pair);
        }
    }
    
    // return freqitems sets
    return fk;
    
}

set<set<int> > apriori_gen_rules(set<set<int> > hm){
    set<set<int> > hm_1;
    
    
    return hm_1;
}


void ap_genrules(set<int> f_k, set<set<int> > hm){
    int k = f_k.size();
    if (hm.size()<=0) {
        return;
    }
    set<set<int> >::iterator iit = hm.begin();
    int m = (*iit).size();
    if (k > m+1) {
        set<set<int> > hm_1 = apriori_gen(hm);
        set<set<int> > modified_hm_1;
        for (set<set<int> >::iterator it = hm_1.begin(); it != hm_1.end(); it++) {
            set<int> small_hm_1 = (*it);
            int sup_count_fk;
            int sup_count_set_diff;
            map<set<int>, int >::iterator it_rules_map = freq_itemsets_support_count_map.find(f_k);
            if (it_rules_map == freq_itemsets_support_count_map.end()) {
                cout << "Error" << endl;
            }else{
                pair<set<int>, int> tt_rulesgen_pair = (*it_rules_map);
                sup_count_fk = tt_rulesgen_pair.second;
            }
            
            // find set difference
            
            set<int> set_diff;
            set_difference(f_k.begin(), f_k.end(), small_hm_1.begin(), small_hm_1.end(),inserter(set_diff, set_diff.end()));
            it_rules_map = freq_itemsets_support_count_map.find(set_diff);
            if (it_rules_map == freq_itemsets_support_count_map.end()) {
                cout << "Error" << endl;
            }else{
                pair<set<int>, int> tt_rulesgen_pair = (*it_rules_map);
                sup_count_set_diff = tt_rulesgen_pair.second;
            }
            
            float conf = (float)sup_count_fk/(float)sup_count_set_diff;
            if (conf >= min_conf) {
                // print rule
                cout << "{ ";
                for (set<int>::iterator ii = set_diff.begin(); ii != set_diff.end(); ii++) {
                    cout << *ii << ",";
                }
                cout << "} -> {";
                for (set<int>::iterator ii = small_hm_1.begin(); ii != small_hm_1.end(); ii++) {
                    cout << *ii << ",";
                }
                cout << "}" << endl;
                modified_hm_1.insert(small_hm_1);
            }
            
            

        }
        ap_genrules(f_k,modified_hm_1);
    }
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
            pair<set<int>, int> my_pair(temp_set,it->second);
            freq_itemsets_support_count_map.insert(my_pair);
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
    
    // get all freq itemsets of size >= 2
    vector<set<int> > freq_itemsets_ex_size_1;
    
    for (int i = 1; i<freq_itemsets.size(); i++) {
        set<set<int> > temp_set_set = freq_itemsets[i];
        for (set<set<int> >::iterator it = temp_set_set.begin(); it != temp_set_set.end(); it++) {
            set<int> temp_set = (*it);
            freq_itemsets_ex_size_1.push_back(temp_set);
        }
    }
    // rule generation
    for (vector<set<int> >::iterator it = freq_itemsets_ex_size_1.begin(); it != freq_itemsets_ex_size_1.end(); it++) {
        cout << "Freq item set size = " << (*it).size() << endl;
        set<set<int> > h1;
        set<int> f_k = (*it);
        set<int> hh1;
        //set<int> modified_hh1;
        for (set<int>::iterator it1 = f_k.begin(); it1 != f_k.end(); it1++) {
            hh1.insert(*it1);
        }
        for (set<int>::iterator ii2 = hh1.begin(); ii2 != hh1.end(); ii2++) {
            set<int> set_difference_fk_hh1;
            set<int> bb_set;
            bb_set.insert(*ii2);
            
            set_difference(f_k.begin(), f_k.end(), bb_set.begin(), bb_set.end(),inserter(set_difference_fk_hh1, set_difference_fk_hh1.end()));
            
            
            int sup_count_fk;
            int sup_count_set_diff;
            map<set<int>, int >::iterator it_rules_map = freq_itemsets_support_count_map.find(f_k);
            if (it_rules_map == freq_itemsets_support_count_map.end()) {
                cout << "Error1" << endl;
            }else{
                pair<set<int>, int> tt_rulesgen_pair = (*it_rules_map);
                sup_count_fk = tt_rulesgen_pair.second;
            }

            it_rules_map = freq_itemsets_support_count_map.find(set_difference_fk_hh1);
            if (it_rules_map == freq_itemsets_support_count_map.end()) {
                cout << "Error2" << endl;
            }else{
                pair<set<int>, int> tt_rulesgen_pair = (*it_rules_map);
                sup_count_set_diff = tt_rulesgen_pair.second;
            }
            
            
            float conf = (float)sup_count_fk/(float)sup_count_set_diff;
            if (conf >= min_conf) {
                // print rule
                cout << "{ ";
                for (set<int>::iterator ii = set_difference_fk_hh1.begin(); ii != set_difference_fk_hh1.end(); ii++) {
                    cout << *ii << ",";
                }
                cout << "} -> {";
                for (set<int>::iterator ii = bb_set.begin(); ii != bb_set.end(); ii++) {
                    cout << *ii << ",";
                }
                cout << "}" << endl;
                h1.insert(bb_set);
            }
        }
        //h1.insert(modified_hh1);
        // calling ap-genrules
        ap_genrules(f_k,h1);
    }
    
    
    
    return 0;
}



