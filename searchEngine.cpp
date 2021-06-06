#include <bits/stdc++.h>
using namespace std;

class URL {
    private: 
        string url;                                         //website url name 
        double rank;                                        //rank of url after iteration 2 of pr algo
        double pr;                                          //page rank of the URL
        double prn;                                         //normalized page rank
        double ct;                                          //the click-through
        double imp;                                         //number of impressions
        double ctr;                                         //the click_through rate
        int edges;                                          //the number of outbound edges the URL has
        vector <string> keywords;                           

    public: 
        URL (string link)
        {
                url = link;
                rank = 0;
                pr = 0;
                prn = 0;
                ct = 0;
                imp = 0;
                ctr = 0;
                edges = 0;
        }
        string getUrl (){
                return url;
        }
        double getRank (){
                return rank;
        }
        void setPageRank (double num){
                pr = num;
        }
        double getPageRank (){
                return pr;
        }
        void setPageRankNormalized (double num){
                prn = num;
        }
        double getPageRankNormalized (){
                return prn;
        }
        double getEdges (){
                return edges;
        }
        void setCT (double num){
                ct = num;
        }
        void incCT (){
                ct++;
        }
        double getCT (){
                return ct;
        }
        void setImpressions (double num){
                imp = num;
        }
        double getImpressions (){
                return imp;
        }
        void incrementImpressions (){
                imp++;
        }
        double incEdges (){
                edges++;
                return edges;
        }
        vector<string> getKeywords() {
            return keywords;
        }
        void computeRank (){
                if (imp !=0) ctr = ct/imp;  
                else ctr = 0;

                double impt = ((0.1*imp)/(1+0.1*imp));
                double temp1 = prn * (1-impt);
                double temp2 = ctr * (impt);
                rank = 0.4*prn + 0.6*(temp1+temp2);
        }

        void insertKeyword (string kw){
            keywords.insert(keywords.end(),kw);
        }

        ~URL (){}
        

};

class WebGraph {
    private: 
        FILE * WebGraphFile;
        FILE * keywordsFile;
        FILE * updatesFile;
        map <string, URL*> wG;
        map <string, vector<string>> adjlist;
        string updateFileArray[100];
        int urlsCount; 
    
    public:
        WebGraph (string wgF, string kwF) { 
            WebGraphFile = fopen (wgF.c_str(), "r");
            keywordsFile = fopen (kwF.c_str(), "r");


            if (WebGraphFile == NULL){
                cout << "ERROR: cannot open WebGraph file: "<< wgF << endl; 
                exit (1);
            }
            if (keywordsFile == NULL){
                cout << "ERROR: cannot open Keywords file: "<< kwF << endl;
                exit (1);
            }
            
            fseek(WebGraphFile, 0, SEEK_SET);

        }

        void createWebGraph () {

            //intializing 
            char edge [100];
            string line, curlink, hyperlink;

            //reading from keywords initialization file
                string link, kw;
                int g = fscanf (keywordsFile, "%[^\n]\n", edge);

                while (g != EOF) {
                        line = edge;
                        stringstream S(edge);
                        getline (S,link,',');
                        wG.insert({link,new URL(link)});
                        vector<string> v;
                        adjlist.insert({link, v});

                        while (getline(S, kw, ',')) {
                            wG[link]->insertKeyword(kw);
                        }

                        g = fscanf (keywordsFile, "%[^\n]\n", edge);
            }

                
            
                //intializing webgraph file
                int f = fscanf (WebGraphFile, "%[^\n]\n", edge);
            
        
                while (f != EOF) {
                    line = edge;
                    stringstream S(line);
                    getline (S,curlink,',');
                    getline (S,hyperlink,'\n');

                    adjlist[curlink].push_back(hyperlink);

                    wG[curlink]->incEdges();

                    f = fscanf (WebGraphFile, "%[^\n]\n", edge);
                }

            
            // set page rank and sort keyowrds for every URL
            double s = wG.size();
            for (auto it = wG.begin(); it != wG.end(); it++) 
                it->second->setPageRank(1.0/s);                     //page rank will be set to 1 at  first


            computePR ();

        }

        void computePR (){ 
            
            // calculate page rank by dividing the page rank of the parent link by their number of edges
            double max = 0;
            
            map <string,double> temp_PR;
            for (auto it = wG.begin(); it != wG.end(); it++) temp_PR.insert({it->first,0});
            
                for (auto it = adjlist.begin(); it != adjlist.end(); it++) {
                    for (auto it1 = adjlist[it->first].begin() ; it1 != adjlist[it->first].end(); it1++) {
                            double x = (wG[it->first]->getPageRank() / adjlist[it->first].size());
                            temp_PR[*it1]+=x;
                        }

                        if (temp_PR[it->first] > max) max = temp_PR[it->first];
                    }
                

                for (auto it = wG.begin(); it != wG.end(); it++)
                        it->second->setPageRank(temp_PR[it->first]);
 
            //set page rank and page rank normalizaed in each URL
            for (auto it = adjlist.begin(); it != adjlist.end(); it++){
                for (auto it1 = adjlist[it->first].begin(); it1 != adjlist[it->first].end(); it1++) {
                    if (temp_PR[*it1] != 0){
                            wG[it->first]->setPageRank(temp_PR[*it1]);
                            wG[it->first]->setPageRankNormalized(temp_PR[*it1]/max);
                    }
                    if (temp_PR[*it1] == 0){
                            wG[it->first]->setPageRankNormalized(wG[it->first]->getPageRank()/max);
                    }

            }
            }
                
        }

        void sort_map (map<double, string>& M) 
        { 

            vector < pair<double, string> > A; 
            map<double, string> :: iterator it;
            for (it=M.begin(); it!=M.end(); it++) 
            {
                A.push_back(make_pair(it->first, it->second));
            }
            
           sort(A.begin(), A.end());  

           M.clear();
            
            vector < pair<double, string> > :: iterator it2;
            for(auto it2=A.begin(); it2!=A.end(); it2++)
                M.insert(pair<double, string> (it2->first, it2->second));

        } 

        void search_AND (string keywords)  
        {
                // create a new URL map and sort it depending on rank
                map <double, string> rankedURL;                 

                vector <string> kw;
                int k=0;
                int pos = 0;
                string token;

                while (pos != std::string::npos) {
                    pos = keywords.find("AND");
                    token = keywords.substr(0, pos);
                    kw.push_back(token);
                    k++;
                    keywords.erase(0, pos + 3);
                }

                //remove double quotes
                for (int i=0; i<k; i++){
                    kw[i].erase(remove(kw[i].begin(), kw[i].end(), '\"'), kw[i].end());
                }

 
                // checks if all keywords exists in a URL and prints if all exists
                map <int, URL*> results;
                int count = 0;

                
                for (auto it = wG.begin(); it != wG.end(); it++){
                        bool successful = true;
                        for (auto it1 = kw.begin(); it1 != kw.end(); it1++){
                            vector <string> vec = it->second->getKeywords();
                            vector<string>::iterator itt = find (vec.begin(), vec.end(), *it1); 
                            if (itt == vec.end()) {
                                successful=false;
                            }
                        }

                        if (successful){
                                it->second->incrementImpressions();

                                it->second->computeRank();

                                double x = it->second->getRank();
                                rankedURL.insert(pair<double, string> (x, it->first));
 
                                results.insert(pair<int,URL*> (count, it->second));
                                count++;
                        }
                        
                }

                if (count==0) cout << "No Search Results! Try Again with another keyword!\n";
                    else {
                    cout << endl;
                    cout << "Search Results: " << endl;
                    sort_map (rankedURL); 
                        
                    int i = 1;

                    for (auto it = rankedURL.rbegin(); it != rankedURL.rend(); ++it)
                    {
                        cout << i << ": " << it->second << "\n";
                        i++;
                    }            
                }
                cout << endl;

                int choice = 0;

                while (choice != 3) { 
                cout << endl;
                cout << "Would you like to\n";
                cout << "1. Choose a webpage to open\n";
                cout << "2. New search\n";
                cout << "3. Exit\n\n";
                cout << "Type in your choice: ";
                cin >> choice;
                cout << endl;

                if (choice == 1) {
                // getting the link number to click on to increment click-through
                if (count){
                        int clickno;
                        cout << "Enter link number you want to click on: \n";
                        cin >> clickno;
                        while (clickno < 1 || clickno > count){
                                string no;
                                cout << "Error: link number does not exist! Re-enter: \n";
                                cin >> no;
                                clickno = stoi(no);
                        }

                        results[clickno-1]->incCT();

                        double x = results[clickno-1]->getCT();                 //get CT for clicked link
                        string s = to_string(x);                                //convert to string to add it to the updates file 
                        ofstream updateFile ("update.txt");                            //open the updates file 

                        for (int i=0; i<urlsCount; i++) {                       //loop over all links, and if found the clicked link, update its CT in the file.
                            string ss = updateFileArray[i].substr(0,13);

                            if (ss == results[clickno-1]->getUrl()) {           //if found the link, update its CT.
                                updateFileArray[i] = results[clickno-1]->getUrl() + "," + s;
                            }

                            if (updateFile.is_open()) updateFile << updateFileArray[i] << endl;

                        }

                        updateFile.close();

                        cin.ignore();

                        cout << endl;
                        cout << "You're now viewing " << results[clickno-1]->getUrl() << "." << endl;

                } else {
                    cout << "There is no search results to open!!\n";
                }

                

            } else if (choice == 2) {
                update("update.txt");
                cout << "Please Enter one or more keywords seperated by AND/OR" << endl;
                string query;
                cin >> query;

                if (query.find("AND") != std::string::npos) {
                    search_AND(query);
                } else {
                    search_OR(query);
                }

            } else if (choice == 3) {
                    cout << "Hope you liked it!\n";
                    exit(1);
            }

        }
    }

        void search_OR (string keywords) 
        {
                // create a new URL vector and sort it depending on rank
                map <double, string> rankedURL;
                
                // parses the strings into seperate words (max 10) also removing quotations
                vector <string> kw;
                int k=0;
                int pos = 0;
                string token;

                while (pos != std::string::npos) {
                    pos = keywords.find("OR");
                    token = keywords.substr(0, pos);
                    kw.push_back(token);
                    k++;
                    keywords.erase(0, pos + 2);
                }

                //remove double quotes
                for (int i=0; i<k; i++){
                    kw[i].erase(remove(kw[i].begin(), kw[i].end(), '\"'), kw[i].end());
                }

                // checks if all keywords exists in a URL and prints if all exists
                
                map <int, URL*> results;
                int count = 0;
                for (auto it = wG.begin(); it != wG.end(); it++){
                        bool successful = false;
                        for (auto it1 = kw.begin(); it1 != kw.end(); it1++){   

                            vector <string> vec = it->second->getKeywords();
                            vector<string>::iterator itt = find (vec.begin(), vec.end(), *it1); 
                            if (itt != vec.end()) successful=true;
                        
                        }   
                        if (successful){
                                it->second->incrementImpressions();

                                it->second->computeRank();
                                
                                double x = it->second->getRank();

                                rankedURL.insert(pair<double, string> (x, it->first));
 
                                results.insert(pair<int,URL*> (count, it->second));
                                count++;
                        }
                
                }

                    if (count==0) cout << "No Search Results! Try Again with another keyword!\n";
                    else {
                    cout << endl;
                    cout << "Search Reuslts: " << endl;
                    sort_map (rankedURL); 
                        
                    int i = 1;

                    for (auto it = rankedURL.rbegin(); it != rankedURL.rend(); ++it)
                    {
                        cout << i << ": " << it->second << "\n";
                        i++;
                    }            
                }
                cout << endl;

                int choice = 0;

                while (choice != 3) {
                cout << endl;
                cout << "Would you like to\n";
                cout << "1. Choose a webpage to open\n";
                cout << "2. New search\n";
                cout << "3. Exit\n\n";
                cout << "Type in your choice: ";
                cin >> choice;
                cout << endl;

                if (choice == 1) {
                // getting the link number to click on to increment click-through
                if (count){
                        int clickno;
                        cout << "Enter link number you want to click on: \n";
                        cin >> clickno;
                        while (clickno < 1 || clickno > count){
                                string no;
                                cout << "Error: link number does not exist! Re-enter: \n";
                                cin >> no;
                                clickno = stoi(no);
                        }

                        results[clickno-1]->incCT();

                        double x = results[clickno-1]->getCT();                 //get CT for clicked link
                        string s = to_string(x);                                //convert to string to add it to the updates file 
                        ofstream updateFile ("update.txt");                            //open the updates file 

                        for (int i=0; i<urlsCount; i++) {                       //loop over all links, and if found the clicked link, update its CT in the file.
                            string ss = updateFileArray[i].substr(0,13);

                            if (ss == results[clickno-1]->getUrl()) {           //if found the link, update its CT.
                                updateFileArray[i] = results[clickno-1]->getUrl() + "," + s;
                            }

                            if (updateFile.is_open()) updateFile << updateFileArray[i] << endl;

                        }

                        updateFile.close();

                        cin.ignore();

                        cout << endl;
                        cout << "You're now viewing " << results[clickno-1]->getUrl() << "." << endl;

                } else {
                    cout << "There is no search results to open!!\n";
                }

                

            } else if (choice == 2) {
                update("update.txt");
                cout << "Please Enter one or more keywords seperated by AND/OR" << endl;

                string query;
                cin >> query;

                if (query.find("AND") != std::string::npos) {
                    search_AND(query);
                } else {
                    search_OR(query);
                }

            } else if (choice == 3) {
                    cout << "Hope you liked it!\n";
                    exit(1);
            }

        }

    }
    

        void update (string updateFileName) {
            updatesFile = fopen (updateFileName.c_str(),"r");

            if (updatesFile == NULL){
                cout << "Unable to open update file!! Terminating\n";
                exit (1);
            }

            //reading from update file
            int imp, upd;
            char lineC [200];
            string cmd, temp, link, lineS;
            int u = fscanf (updatesFile, "%[^\n]\n", lineC);

            urlsCount = 0;
     
            while (u != EOF) {

                lineS = lineC;
                updateFileArray[urlsCount] = lineS;
                urlsCount++;

                stringstream S(lineS);
                getline (S,link,',');
                getline (S,temp,',');
                imp = stoi(temp);
                upd = stoi(temp);

                wG[link]->setImpressions(imp);
                wG[link]->setCT(upd);

                u = fscanf (updatesFile, "%[^\n]\n", lineC);
            }
            
        }

        ~WebGraph ()    //Time complexity: 0(1)
        {
            if (WebGraphFile != NULL) fclose (WebGraphFile);
            if (keywordsFile != NULL) fclose (keywordsFile);
            if (updatesFile != NULL) fclose (updatesFile);
        }

}; 

int main () {

    WebGraph wg ("web_graph.txt", "keywords.txt");

    wg.createWebGraph();

    wg.update("update.txt");    

    int choice = 0;

    cout << "Welcome!\nWhat would you like to do?\n";
    cout << "1. New Search\n";
    cout << "2. Exit\n\n";
    cout << "Type in your choice: ";

    cin >> choice;

    cout << endl;
    
    if (choice == 1) {
        cout << "Please Enter one or more keywords seperated by AND/OR" << endl;


        string query;
        cin >> query;

        if (query.find("AND") != std::string::npos) {
            wg.search_AND(query);
        } else {
            wg.search_OR(query);
        } 



    } else if (choice == 2) {
        cout << "Hope you liked it!\n";
    }

    return 0;
}