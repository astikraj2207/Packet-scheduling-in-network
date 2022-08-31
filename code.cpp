#include <bits/stdc++.h>
using namespace std;
#define ll long long int
#define f(i,a,b) for(int i=a;i<b;i++)
#define F first
#define S second
#define endl '\n' 
#define ce(ele) cout<<ele<<' '
#define CASE(t) int t; cin>>t; while(t--)
#define sor(v) sort(v.begin(),v.end())
#define rev(v) reverse(v.begin(),v.end())
#define vi vector<ll>
#define pb push_back
#define cs(ele) cout<<ele<<'\n'
const ll mod=1e9 +7;

// packet attributes
struct packet
{
    int id;// packet id
    bool is_generated; //if packet is generated or not
    int ip; //input port
    int op; //output port
    double gen_time; //geneartion tym of packet
    double comp_time; //completion time of transmission
    //We will calculate delay by differenece of above two
};

vector<queue<packet>> input_ports;
vector<queue<packet>> output_ports;

int switch_port_count = 8; //number of switch port count 
int buffer_size = 4; //buffer size 
float packet_gen_prob = 0.5; //packet generation probabiolity
char queue_type[20] = "INQ"; //queue type
int max_time_slots = 10000; //number of time slots for simulation
double k_knockout = 0.6; //value of K(defalut is 0.6)
string out_file = "out.txt"; //default output file
// timer represents the timeslots
long long int timer=0;

// packet id
int packet_id;

// comparator for sorting packets by arrival time
bool compare_packet(packet a, packet b){
    return (a.gen_time < b.gen_time);
}
// whether packet would be generated or not
bool is_generated()
{
    double p=packet_gen_prob;
    double prob = ((double) rand() / (RAND_MAX)); //genearte a random no. between 0 and 1
	if(prob>=p)return false;
	return true;
}
// randomly chooses k numbers out of n
vector<int> choose_k(int n,int k){
    if(k>n){
        cout<<"Error in choosing k out of n(n is smaller than k)";
        vector<int> v;
        return v;
    }
    // randomly chooses one and swaps with the last one
    vector<int> v(n);
    f(i,0,n)v[i]=i;
    vector<int> res;
    int sz=n;   
     // time complexity in O(k)
    while(res.size()<k && sz){
        int ind=rand()%sz;
        res.push_back(v[ind]);
        swap(v[ind],v[sz-1]);
        sz--;
    }
    return res;
}
// generates packet
packet generate_packet(int ip)
{
        // check if packet is generated
        int t=timer;
        packet pkt;
        int op = rand() % switch_port_count; //selecting a random output port 
        double gen_time = t + (double)((rand() % 10 + 1) / (1000.0)); //generation time selected randomly between t+0.001 to t+0.01
        pkt.is_generated=is_generated();
        //setting claculated params to packet parameteers
        pkt.ip = ip;// ip as well as output port are 0 indexed
        pkt.gen_time = gen_time;
        pkt.op = op;
        pkt.id=packet_id;
        packet_id++;
        return pkt;
}
// returns heading titles for inq
string title_string_inq(){
    string s="Port Count";
    while(s.size()<15)s.pb(' ');
    s+="Packet gen prob";
    while(s.size()<40)s.pb(' ');
    s+="Queue Type ";
    while(s.size()<60)s.pb(' ');
    s+="Avg delay ";
    while(s.size()<80)s.pb(' ');
    s+="Std Deviation ";
    while(s.size()<105)s.pb(' ');
    s+="Link util ";
    while(s.size()<130)s.pb(' ');
    s+="\n";
    return s;
}
// returns heading titles for kouq
string title_string_kouq(){
    string s="Port Count";
    while(s.size()<15)s.pb(' ');
    s+="Packet gen prob";
    while(s.size()<40)s.pb(' ');
    s+="Queue Type ";
    while(s.size()<60)s.pb(' ');
    s+="Avg delay ";
    while(s.size()<80)s.pb(' ');
    s+="Std Deviation ";
    while(s.size()<105)s.pb(' ');
    s+="Link util ";
    while(s.size()<130)s.pb(' ');
    s+="Drop Probability ";
    while(s.size()<160)s.pb(' ');
    s+="\n";
    return s;
}
// packet scheduling for inq queue
void packet_scheduling_inq()
{   
    srand(time(0));// after using this, rand() function will generate random numbers
    
    ll delay = 0; //delay variable that stores delay sum over all simulation time
    int no_transmitted_packet = 0; //to store number of transmitted packet overall
    int no_gen_pkt=0;//to store number of genearetd packet
    vector < int > delay_arr; //delay vector to calculate standard deviation
    vector<int> port_util(switch_port_count,0);
    timer=0;
    while(timer<max_time_slots){
        map <int,vector<packet>> m;
        for(int i=0;i<switch_port_count;i++){
            packet p=generate_packet(i);
            p = generate_packet(i);
            if (p.is_generated) {
                no_gen_pkt++; //if packet is genearted then we are putting in input queue
                if (input_ports[i].size() < buffer_size)input_ports[i].push(p);

            }
        }
        // inserting in map according to their output port
        for(int i=0;i<switch_port_count;i++)
        {   
            if(input_ports[i].size())
            {
                int tempid=input_ports[i].front().id;
                int tempop=input_ports[i].front().op;
                m[tempop].push_back(input_ports[i].front());
            }
        }
        // randomly choosing one incoming packets from each output port
        for(int i=0;i<switch_port_count;i++)
        {
            // if output port has some packets
            if(m[i].size())
            {
                int tempsize=m[i].size();
                int tempidx=rand()%tempsize;
                packet tempPacket=m[i][tempidx];
                int tempip=tempPacket.ip;
                int tempop=tempPacket.op;
                // removing packets from input ports
                input_ports[tempip].pop();
                tempPacket.comp_time=timer+1;
                no_transmitted_packet++;
                port_util[i]++;
                delay_arr.push_back((int)tempPacket.comp_time - (int)tempPacket.gen_time);
                delay+=((int)tempPacket.comp_time - (int)tempPacket.gen_time);
            }
        }
        timer++;
    }
    // adding output to file
    ofstream fout;
    fout.open(out_file, ios::app); //opening the file in append mode
    ll total_port_util=0;
    for(int i=0;i<switch_port_count;i++){
        total_port_util+=port_util[i];
    }
    double Average_delay = (delay * 1.0) / (double)(delay_arr.size()); //calculating average dealy for overall simulation
    double stand_dev = 0; //variable to store standard deviation
    // calculating deviation
    for (int i = 0; i < delay_arr.size(); i++) {
        stand_dev += pow(((double) delay_arr[i] - Average_delay), 2);
    }

    stand_dev = sqrt(stand_dev / delay_arr.size()); //standard deviation calculated

    float link_utiliization = total_port_util / (switch_port_count * max_time_slots * 1.0); //calculaitng link utilization
    //inserting the entries in the output file
    string title=title_string_inq();
    fout<<title;
    string s="";
    s+=(to_string(switch_port_count));while(s.size()<15)s.pb(' ');
    s+=(to_string(packet_gen_prob));while(s.size()<40)s.pb(' ');
    s+=(queue_type);while(s.size()<60)s.pb(' ');
    s+=(to_string(Average_delay));while(s.size()<80)s.pb(' ');
    s+=(to_string(stand_dev));while(s.size()<105)s.pb(' ');
    s+=(to_string(link_utiliization));while(s.size()<130)s.pb(' ');
    s+="\n";
    fout<<s;
    fout.close(); //closing the file
}
// kouq scheduling
void packet_scheduling_kouq()
{
    ll no_transmitted_packet = 0; //to store number of transmitted packet overall
    ll no_gen_pack = 0; //to store number of genearetd packet
    ll packets_dropped=0;
    double drop_prob = 0; //variable to store drop prob
    ll delay = 0; //delay variable that stores delay sum over all simulation time
    vector < int > delay_arr; //delay vector to claculate std. deviation
    vector<int> port_util(switch_port_count,0); //array to calculate link utilization
    srand(time(0)+100);
    timer=0;
    while(timer < max_time_slots)
    { //loop is running for overall simulation time
        vector <packet> hol; //vector to store  gen packet in  current time slot
        for (int i = 0; i < switch_port_count; i++) 
        {
            //for each input port we are generating a packet 
            packet p = generate_packet(i);
            if (p.is_generated) {
                no_gen_pack++; //if packet is genearted then we r putting in input queue
                if ((int)input_ports[i].size() == 0){
                    input_ports[i].push(p);
                }else{
                    cout<<"Error: input port have size\n";
                }
            }

            if (input_ports[i].size() > 0){
                hol.push_back(input_ports[i].front()); //pushing the current packets in hol vector
            }
        }
        sort(hol.begin(), hol.end(), compare_packet); //sorting the entries in increasing order of gen time
        map<int,vector<packet>> m;
        // pushing packets 
        for(int i=0;i<hol.size();i++){
            m[hol[i].op].push_back(hol[i]);
        }
        ll packet_dropped_due_to_overflow=0;
        // choosing packets to be pushed in output queue
        for(int i=0;i<switch_port_count;i++)
        {
            if(m[i].size())
            {
                // if no. of incoming packets are less than k*n
                if(m[i].size() < (int)((double)k_knockout * (int)switch_port_count)){
                    for(int j=0;j<m[i].size();j++){
                        if(output_ports[i].size()<buffer_size){
                            output_ports[i].push(m[i][j]);
                            port_util[i]++;
                        }else{
                            packet_dropped_due_to_overflow++;
                        }
                    }
                }else{
                    // randomly choosing k elemnts out of all incoming packts at a port
                    vector<int> v=choose_k(m[i].size(),(int)((double)k_knockout * (int)switch_port_count));
                    for(int j=0;j<v.size();j++){
                        if(v[j]>=m[i].size()){
                            cout<<"error here in choose k";
                        }
                        if(output_ports[i].size()<buffer_size){
                            output_ports[i].push(m[i][v[j]]);
                            port_util[i]++;
                        }else{
                            packet_dropped_due_to_overflow++;
                        }
                    }
                    // counting dropped packets
                    packets_dropped+=((int)m[i].size()-(int)((double)k_knockout * (int)switch_port_count));
                }
            }
        }
        for(int i=0;i<switch_port_count;i++){
            while(input_ports[i].size()>0)input_ports[i].pop();
        }
        //Packet Processing
        for (int i = 0; i < switch_port_count; i++) {
            // removing elements from output buffer
            if (output_ports[i].size() > 0) {
                packet p = output_ports[i].front();
                p.comp_time = timer + 1; //setting completion time of packet
                output_ports[i].pop(); //popping that packet from op queue
                delay += (int) p.comp_time - (int) p.gen_time; //calculating delay
                delay_arr.push_back((int) p.comp_time - (int) p.gen_time); //pushing delay into delay array
                no_transmitted_packet++;
            }
        }
        timer++;
    }
    ll sum = 0;
    for (int i = 0; i < switch_port_count; i++) {
        sum += port_util[i]; //calculating link utilization
    }
    double avg_delay = delay / (no_transmitted_packet * 1.0); //calculating Overall delay

    double std_dev = 0; //to store std deviation of delay
    for (int i = 0; i < delay_arr.size(); i++) {
        std_dev += (delay_arr[i] - avg_delay) * (delay_arr[i] - avg_delay);
    }

    ofstream fout;
    fout.open(out_file, ios::app); //opening output file in append mode

    std_dev = sqrt(std_dev /((double) delay_arr.size())); //calculating standard deviation

    double link_utiliization = (double)sum / (switch_port_count * max_time_slots * 1.0); //calculating link utilizatiomn
    //inserting entries into output file
    fout<<endl;
    fout<<title_string_kouq();
    string s="";
    s+=(to_string(switch_port_count));while(s.size()<15)s.pb(' ');
    s+=(to_string(packet_gen_prob));while(s.size()<40)s.pb(' ');
    s+=(queue_type);while(s.size()<60)s.pb(' ');
    s+=(to_string(avg_delay));while(s.size()<80)s.pb(' ');
    s+=(to_string(std_dev));while(s.size()<105)s.pb(' ');
    s+=(to_string(link_utiliization));while(s.size()<130)s.pb(' ');
    drop_prob = (double)packets_dropped / (double)max_time_slots; //calculting drop probabaility
    drop_prob=drop_prob/(double)switch_port_count;
    s+=to_string(drop_prob);
    s+="\n";
    // adding elements in output file
    fout<<s;

    fout.close();
}

// islip phases
// Phase 1: generate packets request to output ports
// Phase 2: grant request
// Phase 3: accept request

void packet_scheduling_islip()
{
    int n=switch_port_count;
    // input queue
    vector<int> link_util(n,0);
    vector<double> delay_vec;
    queue<packet> inp_queue[n][n];
    vector<int> input_arbiter(n, 0);
    vector<int> output_arbiter(n, 0);
    vector<int> size_cnt(n,0);
    // for each time slot
    for(int time=0;time<max_time_slots;time++)
    {
        // generating packets for each input port
        for(int port_number=0;port_number<n;port_number++)
        {
            // generating packet
            packet p=generate_packet(port_number);
            if(p.is_generated)
            {
                p.gen_time=time;
                // psuhing packet in input queue
                if(size_cnt[port_number]<buffer_size){
                    inp_queue[port_number][p.op].push(p);
                    size_cnt[port_number]++;
                }
            }
        }
        vector<int> grants(n, -1);

        //sending grants
        for(int port_number=0;port_number<n;port_number++)
        {
            for(int i=0;i<n;i++)
            {
                // changing input port pointer
                int inputport_number=(i+output_arbiter[port_number])%n;
                if(inp_queue[inputport_number][port_number].size()>0)
                {
                    grants[port_number]=inputport_number;
                    break;
                }
            }
        }
        
        //accepting grants
        for(int port_number=0;port_number<n;port_number++)
        {
            // for all the requests
            for(int i=0;i<n;i++)
            {
                // changing the output port pointer since packet has been accepted to transmit
                int outputport_number=(i+input_arbiter[port_number])%n;
                if(grants[outputport_number]==port_number)
                {
                    // removing the packet to be transmitted from input queue
                    packet p=inp_queue[port_number][outputport_number].front();
                    inp_queue[port_number][outputport_number].pop();
                    size_cnt[port_number]--;
                    // setting the completion time and claculating various paramters
                    p.comp_time=time+1;
                    delay_vec.push_back(p.comp_time-p.gen_time);
                    link_util[p.ip]++;
                    input_arbiter[port_number]=(outputport_number+1)%n;
                    output_arbiter[outputport_number]=(port_number+1)%n;
                    break;
                }
            }
        }
    }
    // calculating the various parameters
    double mean=0;
    for(auto x: delay_vec){
        mean=mean+x;
    }
    mean=mean/(double)(delay_vec.size()); // calculati ng mean
    double link_=0;
    
    for(auto x: link_util){ // calculating total utility
        link_ += x;
    }
    link_ = (link_)/(max_time_slots*n); // calculating link utilization
    
    double std_dev = 0,avg_delay=mean;
    for(auto x: delay_vec){
        std_dev += (x-mean)*(x-mean);
    }
    std_dev /= (delay_vec.size());
    std_dev = sqrt(std_dev);
    ofstream fout;
    fout.open(out_file, ios::app); //opening output file in append mode 
    fout<<endl;
    fout<<title_string_inq(); // printing headers for each parameter
    string s="";
    s+=(to_string(switch_port_count));while(s.size()<15)s.pb(' ');
    s+=(to_string(packet_gen_prob));while(s.size()<40)s.pb(' ');
    s+=(queue_type);while(s.size()<60)s.pb(' ');
    s+=(to_string(avg_delay));while(s.size()<80)s.pb(' ');
    s+=(to_string(std_dev));while(s.size()<105)s.pb(' ');
    s+=(to_string(link_));while(s.size()<130)s.pb(' ');
    s+="\n";
    // appending the calculated parameters in output file
    fout<<s;
    fout.close();

}
 
int main(int argc, char ** argv) {
    ios::sync_with_stdio(0);
    cin.tie(0);cout.tie(0);
    // resloving input from command line
    if (argc <= 15) {
        int i=1;
        while(i<argc){
            if (strcmp(argv[i], "-N") == 0) {
                switch_port_count = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-T") == 0) {
                max_time_slots = stoi(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-p") == 0) {
                packet_gen_prob = stof(argv[i + 1]);
            } else if (strcmp(argv[i], "-queue") == 0) {
                int j = 0;
                while ((argv[i + 1])[j] != '\0'){
                    queue_type[j] = (argv[i + 1])[j], j++;
                }
            } else if (strcmp(argv[i], "-B") == 0) {
                buffer_size = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-K") == 0) {
                k_knockout = stof(argv[i + 1]);
            } else if (strcmp(argv[i], "-out") == 0) {
                out_file = argv[i + 1];
            }else {
                cout << "Invalid Input" << endl;
                return 0;
            }
            i+=2;
        }
        // initialising global variables
        for(int i=0;i<switch_port_count;i++)
        {
            queue<packet> x;
            input_ports.push_back(x);
            output_ports.push_back(x); 
        }
        timer=0;packet_id=0; //initialising timer and packet id to zero

        if (strcmp("INQ", queue_type) == 0) { // calling function for respective queue type
            packet_scheduling_inq();
            cout << "\n See " << out_file << " for results\n";
        } else if (strcmp("KOUQ", queue_type) == 0) {
            packet_scheduling_kouq();
            cout << "\n See " << out_file << " for results\n";
        } else if (strcmp("ISLIP", queue_type) == 0) {
            packet_scheduling_islip();
            cout << "\n See " << out_file << " for results\n";
        } else {
            cout << "Queue can be only INQ / KOUQ / ISLIP";
            cout << "\nProgram is terminating";
            return 0;
        }
    } else {
        cout << "Wrong input format!!" << endl;
    }
    return 0;
}
