#include <cstdlib>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

/*
 * 
 */


#define num_endnodes 36
#define num_LeafSwitch_ports 5
#define num_AggrSwitch_ports 9
#define num_CoreSwitch_ports 8
#define num_lft_entery 36
#define num_leaf_switch 12
#define num_aggr_switch 8
#define num_core_switch 4
#define number_leaf_clients 3
#define num_ports_aggr_switch 9
#define num_vm 200
#define num_leaf_UpGroup 2
#define num_leaf_DownGroup 3
#define num_aggr_UpGroup 4
#define num_aggr_DownGroup 3
#define num_switches 24
#define num_DownGroup_MAX 8
#define num_UpGroup_MAX 4
#define num_switch_ports_MAX 9
#define UpGroup 0
#define DownGroup 1
#define BW_leaf_switch 2300
#define BW_aggr_switch 45000
#define BW_core_switch 80000
#define schedule_time 20

struct Port
{	
	int down_weight;
	int up_weight;
	int witch_switch; //ID switch nazir
	int lid;
        int guid;
};

struct Client
{
	int lid;
	int rcv_weight;
	int port_connected;
	int leaf_switch;  
        int in_traf;
        int out_traf;        
};

struct Lft
{
    int dest_host;
    int out_port;
};

struct PortSwitch
{
    int next_switch_num;
    int next_port_num;
};
struct Switch_struct 
{
	PortSwitch UpGroups[num_UpGroup_MAX]; //grouhe porthayee ke switch dar level badi beheshoon motasele
        PortSwitch DownGroups[num_DownGroup_MAX]; //grouhe porthayee ke switch dar level gahbli beheshoon motasele
	int number_of_ports; //switch chand porte?
	Lft lft[num_lft_entery]; // jadvale masiryabi switch
	Client connected_clients[number_leaf_clients]; //goohe client haye motasel be in switch ke agar leaf switch bashe meghdar migire
        Port port[num_switch_ports_MAX];
        int level; // core level==0 core level==1   core level==2        
};

// creating VMs
struct Vm
{
    int in_traf;
    int out_traf;
    int host;
};



//Sorting nodes of a leaf switch by decreasing weights
void sort_by_rcv_weight(Client host[num_endnodes], int num_host)
{    
        int i, j;
        Client t;
        int k=0;   
        
        while (k<36)
        {
            for(i=k; i<(k+3) ; i++)
            {
                t=host[i];                
                
                for(j=i; j>k; j--)
                    {
                        if(t.rcv_weight<=host[j-1].rcv_weight)
                            break;
                        /*host[j].in_traf=host[j-1].in_traf;
                        host[j].leaf_switch=host[j-1].leaf_switch;
                        host[j].lid=host[j-1].lid;
                        host[j].out_traf=host[j-1].out_traf;
                        host[j].rcv_weight=host[j-1].rcv_weight;*/
                        host[j]=host[j-1];
                    }
                host[j]=t;
                    
                   /* host[j].in_traf=t.in_traf;
                    host[j].leaf_switch=t.leaf_switch;
                    host[j].lid=t.lid;
                    host[j].out_traf=t.out_traf;
                    host[j].rcv_weight=t.rcv_weight;
                    //host[j].port_connected=t.port_connected;
                    */
            }
            k+=3;
        }        
}


PortSwitch GetLeastLoadedPort(Switch_struct s[num_switches],int index,int UpOrDown)
{   
        
    if(!UpOrDown)
    {
        int min_index=0;
        int i;
        PortSwitch min_up_port;
        int min_down_weight;
        int min_up_weight;
        int UpGroup_down_weight;
        int UpGroup_up_weight;
        int min_GUID;
        int UpGroup_GUID;
        
        min_down_weight=s[s[index].UpGroups[0].next_switch_num].port[s[index].UpGroups[0].next_port_num].down_weight;
        min_up_weight=s[s[index].UpGroups[0].next_switch_num].port[s[index].UpGroups[0].next_port_num].up_weight;
        min_GUID=s[s[index].UpGroups[0].next_switch_num].port[s[index].UpGroups[0].next_port_num].guid;



        if (index>=0 && index<num_leaf_switch)
            {

            for(i=1; i<num_leaf_UpGroup ;i++)
                {
                    UpGroup_down_weight=s[s[index].UpGroups[i].next_switch_num].port[s[index].UpGroups[i].next_port_num].down_weight;
                    UpGroup_up_weight=s[s[index].UpGroups[i].next_switch_num].port[s[index].UpGroups[i].next_port_num].up_weight;
                    UpGroup_GUID=s[s[index].UpGroups[i].next_switch_num].port[s[index].UpGroups[i].next_port_num].guid;
                    if ( UpGroup_down_weight < min_down_weight)
                    {
                        min_index=i;
                        min_down_weight = UpGroup_down_weight;
                    }
                    else  if (min_down_weight == UpGroup_down_weight)
                    {        
                        if (UpGroup_up_weight < min_up_weight)
                        {
                            min_index=i;
                            min_down_weight = s[s[index].UpGroups[i].next_switch_num].port[s[index].UpGroups[i].next_port_num].down_weight;
                        }
                        else if (UpGroup_up_weight == min_up_weight)
                        {
                            if (UpGroup_GUID<min_GUID)
                            {
                                min_index=i;
                                min_down_weight = s[s[index].UpGroups[i].next_switch_num].port[s[index].UpGroups[i].next_port_num].down_weight;
                            }
                        }                
                    }
                }
             }

        if ( index>=num_leaf_switch && index < (num_leaf_switch+num_aggr_switch) )
            {

            for(i=1; i<num_aggr_UpGroup ;i++)
                {
                    UpGroup_down_weight=s[s[index].UpGroups[i].next_switch_num].port[s[index].UpGroups[i].next_port_num].down_weight;
                    UpGroup_up_weight=s[s[index].UpGroups[i].next_switch_num].port[s[index].UpGroups[i].next_port_num].up_weight;
                    UpGroup_GUID=s[s[index].UpGroups[i].next_switch_num].port[s[index].UpGroups[i].next_port_num].guid;
                    if ( UpGroup_down_weight < min_down_weight)
                    {
                        min_index=i;
                        min_down_weight = UpGroup_down_weight;
                    }
                    else  if (min_down_weight == UpGroup_down_weight)
                    {        
                        if (UpGroup_up_weight < min_up_weight)
                        {
                            min_index=i;
                            min_down_weight = s[s[index].UpGroups[i].next_switch_num].port[s[index].UpGroups[i].next_port_num].down_weight;
                        }
                        else if (UpGroup_up_weight == min_up_weight)
                        {
                            if (UpGroup_GUID<min_GUID)
                            {
                                min_index=i;
                                min_down_weight = s[s[index].UpGroups[i].next_switch_num].port[s[index].UpGroups[i].next_port_num].down_weight;
                            }
                        }                
                    }
                }
             }
        min_up_port=s[index].UpGroups[min_index];
        return min_up_port;   
    }
    
    else if(UpOrDown)
        
    {        
        int min_index=0;
        int i;
        PortSwitch min_down_port;
        int min_down_weight;
        int min_up_weight;
        int DownGroup_down_weight;
        int DownGroup_up_weight;
        int min_GUID;
        int DownGroup_GUID;
        
        min_down_weight=s[s[index].DownGroups[0].next_switch_num].port[s[index].DownGroups[0].next_port_num].down_weight;
        min_up_weight=s[s[index].DownGroups[0].next_switch_num].port[s[index].DownGroups[0].next_port_num].up_weight;
        min_GUID=s[s[index].DownGroups[0].next_switch_num].port[s[index].DownGroups[0].next_port_num].guid;

        if ( index>=num_leaf_switch && index < (num_leaf_switch+num_aggr_switch) )
            {

            for(i=1; i<num_aggr_UpGroup ;i++)
                {
                    DownGroup_down_weight=s[s[index].DownGroups[i].next_switch_num].port[s[index].DownGroups[i].next_port_num].down_weight;
                    DownGroup_up_weight=s[s[index].DownGroups[i].next_switch_num].port[s[index].DownGroups[i].next_port_num].up_weight;
                    DownGroup_GUID=s[s[index].DownGroups[i].next_switch_num].port[s[index].DownGroups[i].next_port_num].guid;
                    if ( DownGroup_down_weight < min_down_weight)
                    {
                        min_index=i;
                        min_down_weight = DownGroup_down_weight;
                    }
                    else  if (min_down_weight == DownGroup_down_weight)
                    {        
                        if (DownGroup_up_weight < min_up_weight)
                        {
                            min_index=i;
                            min_down_weight = s[s[index].DownGroups[i].next_switch_num].port[s[index].DownGroups[i].next_port_num].down_weight;
                        }
                        else if (DownGroup_up_weight == min_up_weight)
                        {
                            if (DownGroup_GUID<min_GUID)
                            {
                                min_index=i;
                                min_down_weight = s[s[index].DownGroups[i].next_switch_num].port[s[index].DownGroups[i].next_port_num].down_weight;
                            }
                        }                
                    }
                }
             }
        if (index>=num_aggr_switch && index<num_switches)
            {
            for(i=1; i<num_DownGroup_MAX ;i++)
                {
                    DownGroup_down_weight=s[s[index].DownGroups[i].next_switch_num].port[s[index].DownGroups[i].next_port_num].down_weight;
                    DownGroup_up_weight=s[s[index].DownGroups[i].next_switch_num].port[s[index].DownGroups[i].next_port_num].up_weight;
                    DownGroup_GUID=s[s[index].DownGroups[i].next_switch_num].port[s[index].DownGroups[i].next_port_num].guid;
                    if ( DownGroup_down_weight < min_down_weight)
                    {
                        min_index=i;
                        min_down_weight = DownGroup_down_weight;
                    }
                    else  if (min_down_weight == DownGroup_down_weight)
                    {        
                        if (DownGroup_up_weight < min_up_weight)
                        {
                            min_index=i;
                            min_down_weight = s[s[index].DownGroups[i].next_switch_num].port[s[index].DownGroups[i].next_port_num].down_weight;
                        }
                        else if (DownGroup_up_weight == min_up_weight)
                        {
                            if (DownGroup_GUID<min_GUID)
                            {
                                min_index=i;
                                min_down_weight = s[s[index].DownGroups[i].next_switch_num].port[s[index].DownGroups[i].next_port_num].down_weight;
                            }
                        }                
                    }
                }
             }
        min_down_port=s[index].DownGroups[min_index];
        return min_down_port;
    }

     
}

void RoutUpGoingByDesc(Switch_struct s[num_switches],Client host[num_endnodes] ,int s_index,int h_index)
{  
    int host_lid=host[h_index].lid;     //to save the lid of demanded host
    if(s[s_index].lft[host_lid].dest_host == -1)
    {
        PortSwitch next_least_loaded_port;                              //to save least loaded down group port
        int next_switch_index;                                          //to save the next switch index
        int next_port; 
       
        next_least_loaded_port = GetLeastLoadedPort(s,s_index,DownGroup);         //find the least loaded port in up group of the current switch 
        next_switch_index = next_least_loaded_port.next_switch_num;
        next_port = next_least_loaded_port.next_port_num;

        s[next_switch_index].lft[host_lid].dest_host=host_lid;          //update the LFT of the next switch
        s[next_switch_index].lft[host_lid].out_port=next_port;

        s[next_switch_index].port[next_port].up_weight+=host[h_index].rcv_weight;     //update the down weight of the founded port of next switch
        
        RoutUpGoingByDesc(s,host,next_switch_index,h_index);
    }
    else return;
}

void RoutDownGoingByAsc(Switch_struct s[num_switches],Client host[num_endnodes] ,int s_index,int h_index)
{
    if (s_index<20)
    {
        PortSwitch next_least_loaded_port;                              //to save least loaded up group port
        int host_lid=host[h_index].lid;                                 //to save the lid of demanded host
        int next_switch_index;                                          //to save the next switch index
        int next_port;                                                  //to save the least loaded port of the next switch index

        next_least_loaded_port = GetLeastLoadedPort(s,s_index,UpGroup);         //find the least loaded port in up group of the current switch 
         next_switch_index = next_least_loaded_port.next_switch_num;
        next_port = next_least_loaded_port.next_port_num;

        s[next_switch_index].lft[host_lid].dest_host=host_lid;          //update the LFT of the next switch
        s[next_switch_index].lft[host_lid].out_port=next_port;
        
        s[next_switch_index].port[next_port].down_weight+=host[h_index].rcv_weight;     //update the down weight of the founded port of next switch
        //cout<<"weight selected port"<<s[next_switch_index].port[next_port].down_weight<<'\n';
        //cout<<"lft switch"<<next_switch_index<<" baraye residan b "<<s[next_switch_index].lft[host_lid].dest_host<<" kharej sho az "<<s[next_switch_index].lft[host_lid].out_port<<'\n';
        RoutUpGoingByDesc(s,host,s_index,h_index);                      //traversing toward the demanded host
        RoutDownGoingByAsc(s,host,next_switch_index,h_index);           // keep going upward
    }
    else return;
}





int main( )
{   
    Switch_struct swtch[num_switches];
    Client end_nodes[num_endnodes];
    
    //variables:
    Vm util[num_vm];    
   
    int traffic[num_vm][num_vm];
	  
    //matrix 2 bodi farzi
    for(int i=0;i<num_vm;i++)
        for(int j=0;j<num_vm;j++)
            if(i!=j)
            traffic[i][j]=rand()%10 ;
            else
                traffic[i][j]=0;
            
    //initialization utilization by VM traffics and hosts
    for(int i=0; i<num_vm; i++)                               
        util[i].host=rand()%36;
    
    for(int i=0; i<num_vm; i++)
        {
            int temp_rcv=0;
            int temp_send=0;
            for(int j=0;j<num_vm;j++)
                {
                    if(util[i].host != util[j].host )
                     {
                             temp_rcv+=traffic[j][i];
                             temp_send+=traffic[i][j];
                     }                      
                }
            util[i].in_traf =temp_rcv ; 
            util[i].out_traf =temp_send ;
           //cout<<"temp recv:"<<temp_rcv<<" "<<"temp send:"<<temp_send<<'\n';
        }
    
     //initialization end nodes
    for(int i=0; i<num_vm; i++)
        {
        end_nodes[util[i].host].in_traf=util[i].in_traf;
        end_nodes[util[i].host].out_traf=util[i].out_traf;
        }   
    
    for(int i=0; i<num_endnodes; i++)
        {       
            end_nodes[i].rcv_weight=(end_nodes[i].in_traf/10);            
        }    
    
    for(int i=0; i<num_endnodes; i++)
        {       
            end_nodes[i].lid=i;
            end_nodes[i].leaf_switch=(i/number_leaf_clients);
            end_nodes[i].port_connected=i%number_leaf_clients;
        }
    
    
    //initialization switches 
    
    for(int i=0;i<num_switches;i++)
    {
        for(int j=0 ; j<num_DownGroup_MAX ; j++)
            {
            swtch[i].DownGroups[j].next_port_num=-1;
            swtch[i].DownGroups[j].next_switch_num=-1;
            }
        for(int j=0 ; j<num_UpGroup_MAX ; j++)
            {
            swtch[i].UpGroups[j].next_port_num=-1;
            swtch[i].UpGroups[j].next_switch_num=-1;
            }
        for(int j=0 ; j<num_UpGroup_MAX ; j++)
            {
            swtch[i].port[j].down_weight=0;
            swtch[i].port[j].up_weight=0;
            }
    }
    
    
    //Leaf Switches:
    int k=0;
    for(int i=0;i<num_leaf_switch;i++)
        {        
            swtch[i].level=2;            
            swtch[i].number_of_ports=num_LeafSwitch_ports;
            for (int j=0; j<number_leaf_clients;j++)
            {
                swtch[i].connected_clients[j].rcv_weight=end_nodes[number_leaf_clients*i+j].rcv_weight;
                swtch[i].connected_clients[j].in_traf=end_nodes[number_leaf_clients*i+j].in_traf;
                swtch[i].connected_clients[j].out_traf=end_nodes[number_leaf_clients*i+j].out_traf;
                swtch[i].connected_clients[j].lid=j;
                swtch[i].connected_clients[j].port_connected=j%3;                
            }       
            
            swtch[i].UpGroups[0].next_port_num=i%3;
            swtch[i].UpGroups[0].next_switch_num=(2*k)+num_leaf_switch;
            swtch[i].UpGroups[1].next_port_num=i%3;
            swtch[i].UpGroups[1].next_switch_num=(2*k)+num_leaf_switch+1;        
            
        if ((i+1)%3 == 0) k++;                 
       
        for (int j=0; j<num_lft_entery;j++)  
            {
                swtch[i].lft[j].dest_host=-1;
                swtch[i].lft[j].out_port=-1;
            }  
        for(int j=0; j<num_LeafSwitch_ports;j++)
            {
            swtch[i].port[j].guid=(i+100);
            swtch[i].port[j].down_weight=0;
            swtch[i].port[j].up_weight=0;
            swtch[i].port[j].lid=j;
            swtch[i].port[j].witch_switch=i;          
            }        
        }
    
    
    //Aggregation Switches:
            
    swtch[12].DownGroups[0].next_port_num=3;
    swtch[12].DownGroups[0].next_switch_num=0;
    swtch[12].DownGroups[1].next_port_num=3;
    swtch[12].DownGroups[1].next_switch_num=1;
    swtch[12].DownGroups[2].next_port_num=3;
    swtch[12].DownGroups[2].next_switch_num=2;
    swtch[13].DownGroups[0].next_port_num=4;
    swtch[13].DownGroups[0].next_switch_num=0;
    swtch[13].DownGroups[1].next_port_num=4;
    swtch[13].DownGroups[1].next_switch_num=1;
    swtch[13].DownGroups[2].next_port_num=4;
    swtch[13].DownGroups[2].next_switch_num=2;
        
    swtch[14].DownGroups[0].next_port_num=3;
    swtch[14].DownGroups[0].next_switch_num=3;
    swtch[14].DownGroups[1].next_port_num=3;
    swtch[14].DownGroups[1].next_switch_num=4;
    swtch[14].DownGroups[2].next_port_num=3;
    swtch[14].DownGroups[2].next_switch_num=5;
    swtch[15].DownGroups[0].next_port_num=4;
    swtch[15].DownGroups[0].next_switch_num=3;
    swtch[15].DownGroups[1].next_port_num=4;
    swtch[15].DownGroups[1].next_switch_num=4;
    swtch[15].DownGroups[2].next_port_num=4;
    swtch[15].DownGroups[2].next_switch_num=5;
    
    swtch[16].DownGroups[0].next_port_num=3;
    swtch[16].DownGroups[0].next_switch_num=6;
    swtch[16].DownGroups[1].next_port_num=3;
    swtch[16].DownGroups[1].next_switch_num=7;
    swtch[16].DownGroups[2].next_port_num=3;
    swtch[16].DownGroups[2].next_switch_num=8;
    swtch[17].DownGroups[0].next_port_num=4;
    swtch[17].DownGroups[0].next_switch_num=6;
    swtch[17].DownGroups[1].next_port_num=4;
    swtch[17].DownGroups[1].next_switch_num=7;
    swtch[17].DownGroups[2].next_port_num=4;
    swtch[17].DownGroups[2].next_switch_num=8;
    
    swtch[18].DownGroups[0].next_port_num=3;
    swtch[18].DownGroups[0].next_switch_num=9;
    swtch[18].DownGroups[1].next_port_num=3;
    swtch[18].DownGroups[1].next_switch_num=10;
    swtch[18].DownGroups[2].next_port_num=3;
    swtch[18].DownGroups[2].next_switch_num=11;
    swtch[19].DownGroups[0].next_port_num=4;
    swtch[19].DownGroups[0].next_switch_num=9;
    swtch[19].DownGroups[1].next_port_num=4;
    swtch[19].DownGroups[1].next_switch_num=10;
    swtch[19].DownGroups[2].next_port_num=4;
    swtch[19].DownGroups[2].next_switch_num=11;
        
    for(int i=num_leaf_switch;i<(num_leaf_switch+num_aggr_switch);i++)
    {
        swtch[i].level=1;        
        swtch[i].number_of_ports=num_ports_aggr_switch;
        
        swtch[i].UpGroups[0].next_switch_num=20;
        swtch[i].UpGroups[0].next_port_num=i-12;
        swtch[i].UpGroups[1].next_switch_num=21;
        swtch[i].UpGroups[1].next_port_num=i-12;
        swtch[i].UpGroups[2].next_switch_num=22;
        swtch[i].UpGroups[2].next_port_num=i-12;
        swtch[i].UpGroups[3].next_switch_num=23;
        swtch[i].UpGroups[3].next_port_num=i-12;
                
        for (int j=0 ; j<num_lft_entery ; j++)
        {
            swtch[i].lft[j].dest_host=-1;
            swtch[i].lft[j].out_port=-1;
        }        
        for (int j=0 ; j<num_AggrSwitch_ports ; j++)
        {            
            swtch[i].port[j].down_weight=0;
            swtch[i].port[j].up_weight=0;
            swtch[i].port[j].lid=j;
            swtch[i].port[j].guid=(i+100);
            swtch[i].port[j].witch_switch=i;
        }
        
           
        for (int j=0 ; j<number_leaf_clients ; j++)
        {
            swtch[i].connected_clients[j].in_traf=-1;
            swtch[i].connected_clients[j].leaf_switch=-1;
            swtch[i].connected_clients[j].lid=-1;
            swtch[i].connected_clients[j].out_traf=-1;
            swtch[i].connected_clients[j].port_connected=-1;
            swtch[i].connected_clients[j].rcv_weight=-1;                    
        }        
    } 
    
    swtch[14].port[2].down_weight=7000;
    swtch[15].port[2].down_weight=7000;
    swtch[14].port[2].up_weight=600;
    swtch[15].port[2].up_weight=600;
    
    
    //initialization core switches
    
    for(int i=(num_leaf_switch+num_aggr_switch);i<num_switches ;i++)
    {       
        swtch[i].level=0;
        swtch[i].number_of_ports=num_CoreSwitch_ports;
        
        for (int j=0 ; j<number_leaf_clients ; j++)
        {
            swtch[i].connected_clients[j].in_traf=-1;
            swtch[i].connected_clients[j].leaf_switch=-1;
            swtch[i].connected_clients[j].lid=-1;
            swtch[i].connected_clients[j].out_traf=-1;
            swtch[i].connected_clients[j].port_connected=-1;
            swtch[i].connected_clients[j].rcv_weight=-1;                    
        }
        
        for (int j=0 ; j<num_lft_entery ; j++)
        {
            swtch[i].lft[j].dest_host=-1;
            swtch[i].lft[j].out_port=-1;
        }
        
        for (int j=0 ; j<8 ; j++)
        {            
            swtch[i].port[j].down_weight=0;
            swtch[i].port[j].up_weight=0;
            swtch[i].port[j].guid=(i+100);
            swtch[i].port[j].lid=j;
            swtch[i].port[j].witch_switch=i;            
        }
        
        for (int j=0 ; j<num_aggr_switch ; j++)
        {
            swtch[i].DownGroups[j].next_port_num=(i-(num_leaf_switch+num_aggr_switch)+3);
            swtch[i].DownGroups[j].next_switch_num=num_leaf_switch+j;
        }           
    }

  
    //sorting hosts by decreasing weight- 1st phase
    
    for(int i=0 ; i<num_leaf_switch ; i++)	
	{          
            for(int j=0 ; j<number_leaf_clients ; j++)
            {
                swtch[i].lft[(3*i)+j].dest_host=end_nodes[(3*i)+j].lid;
                swtch[i].lft[(3*i)+j].out_port=end_nodes[(3*i)+j].port_connected;
                swtch[i].port[j].down_weight=end_nodes[(3*i)+j].rcv_weight;
            }                        
        }
    sort_by_rcv_weight(end_nodes,num_endnodes);  //sort kardane end nodhaye yek switch bar asase rcv weight (nozuli)                   
    
    for(int i=0 ; i<num_leaf_switch ; i++)	
	{          
            for(int j=0 ; j<number_leaf_clients ; j++)
                {
                RoutDownGoingByAsc(swtch,end_nodes,i,((3*i)+j));                
                }                        
        }
    
    /*
    //testing the initialization leaf switches    
    for(int i=0; i<num_leaf_switch ; i++)	
	{
        cout<<"LFT LeafSwitch "<<i<<" : \n";    
        for(int j=0 ; j<num_lft_entery ; j++)
            //    cout<<j<<"="<<leaf_switches[i].connected_clients[j].rcv_weight<<'\t';                        
            cout<<j<<" )"<<"baraye residan b host "<<swtch[i].lft[j].dest_host <<" out port is : "<< swtch[i].lft[j].out_port <<"\n";
        cout<<"___________________________________________________________________\n\n";
        }
    
    //testing the initialization aggr switches    
    for(int i=num_leaf_switch; i<num_leaf_switch+num_aggr_switch ; i++)	
	{
        cout<<"LFT AggreSwitch "<<i<<" : \n";    
        for(int j=0 ; j<num_lft_entery ; j++)
            //    cout<<j<<"="<<leaf_switches[i].connected_clients[j].rcv_weight<<'\t';                        
            cout<<j<<" )"<<"baraye residan b host "<<swtch[i].lft[j].dest_host <<" out port is : "<< swtch[i].lft[j].out_port <<"\n";
        cout<<"___________________________________________________________________\n\n";
        }
    
    //testing the initialization core switches
    for(int i=num_leaf_switch+num_aggr_switch; i<num_switches ; i++)	
	{
        cout<<"LFT Core Switch "<<i<<" : \n";    
        for(int j=0 ; j<num_lft_entery ; j++)
            //    cout<<j<<"="<<leaf_switches[i].connected_clients[j].rcv_weight<<'\t';                        
            cout<<j<<" )"<<"baraye residan b host "<<swtch[i].lft[j].dest_host <<" out port is : "<< swtch[i].lft[j].out_port <<"\n";
        cout<<"___________________________________________________________________\n\n";
        }
     */ 
    int total_weight;
    int total_Energy;
    for(int i=0 ; i<num_switches ; i++)
    {
        total_weight=0;        
        total_Energy=0;
        
        if (i<num_leaf_switch)
        {
        for(int j=0 ; j<num_CoreSwitch_ports ; j++)
            total_weight+=(swtch[i].port[j].down_weight+swtch[i].port[j].up_weight);
        total_Energy=total_weight*100000 /(BW_core_switch*schedule_time);
        cout<<"total Energy of core-switch "<<i<<" is "<<total_Energy<<'\n';
        cout<<"- - - - - - - - - - - - - - - -\n\n";
        }
        else if(i<(num_leaf_switch+num_aggr_switch))
        {
        for(int j=0 ; j<num_CoreSwitch_ports ; j++)
            total_weight+=(swtch[i].port[j].down_weight+swtch[i].port[j].up_weight);
        total_Energy=total_weight*100000 /(BW_core_switch*schedule_time);
        cout<<"total Energy of core-switch "<<i<<" is "<<total_Energy<<'\n';
        cout<<"- - - - - - - - - - - - - - - -\n\n";
        }
        else
        {
        for(int j=0 ; j<num_CoreSwitch_ports ; j++)
            total_weight+=(swtch[i].port[j].down_weight+swtch[i].port[j].up_weight);
        total_Energy=total_weight*100000 /(BW_core_switch*schedule_time);
        cout<<"total Energy of core-switch "<<i<<" is "<<total_Energy<<'\n';
        cout<<"- - - - - - - - - - - - - - - -\n\n";
        }        
    }
}

