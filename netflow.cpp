#include "netflow.hpp"
#include <stdexcept>
#include "queue"
#include <iostream>
#include <climits>

#define TOO_FEW_VERTICES "Too few vertices."
#define TOO_FEW_EDGES "Too few edges."
#define EDGE_WEIGHT_ZERO "Detected edge weight of 0."
#define EDGE_BAD_ENDPOINT "Edge interacts with nonexistent vertex."
#define SELF_LOOP "At least one self-loop."
#define MULTI_EDGES "Detected multi-edges."
#define NOT_ONE_SRC "Zero or more than one source."
#define NOT_ONE_SINK "Zero or more than one sink."

int check(const std::vector<Edge>& flowNet,
    unsigned int numberOfVertices)
{
    if(numberOfVertices < 2)
    {
       throw std::runtime_error(TOO_FEW_VERTICES);
    }
    else if(flowNet.size() == 0)
    {
        throw std::runtime_error(TOO_FEW_EDGES);
    }
    else
    {
        for(unsigned int i=0; i < flowNet.size(); i++)
        {
            if(flowNet[i].weight == 0)
            {
                throw std::runtime_error(EDGE_WEIGHT_ZERO);
            }
        }

        for(unsigned int i=0; i < flowNet.size(); i++)
        {
            if(flowNet[i].from >= numberOfVertices || flowNet[i].to >= numberOfVertices)
            {
                throw std::runtime_error(EDGE_BAD_ENDPOINT);
            }
        }

        for(unsigned int i=0; i < flowNet.size(); i++)
        {
            if(flowNet[i].from == flowNet[i].to)
            {
                throw std::runtime_error(SELF_LOOP);
            }
        }

        for(unsigned int i=0; i < flowNet.size(); i++)
        {
           for(unsigned int j=i+1; j < flowNet.size(); j++)
           {
             if(flowNet[i].from == flowNet[j].from && flowNet[i].to == flowNet[j].to)
             {
                throw std::runtime_error(MULTI_EDGES);
             }
           }
        }

        unsigned int count = 0;
        unsigned int found = 0;
        unsigned int source = -1;

        for(unsigned int i=0; i < flowNet.size(); i++)
        {
            count = 0;
            if(flowNet[i].from == source)
            {
                continue;
            }

           for(unsigned int j=0; j < flowNet.size(); j++)
           {
             if(flowNet[i].from == flowNet[j].to)
             {
                count++;
                break;
             }
           }

           if(count == 0)
           {
               found++;
               source = flowNet[i].from;
           }
        }

        if(found != 1)
        {
            throw std::runtime_error(NOT_ONE_SRC);
        }

        unsigned int countSink = 1;
        unsigned int foundSink = 0; 
        unsigned int sink = -1;

        for(unsigned int i=0; i < flowNet.size(); i++)
        {
           countSink = 0;

            if(flowNet[i].to == sink)
            {
                continue;
            }


           for(unsigned int j=0; j < flowNet.size(); j++)
           {
             if(flowNet[i].to == flowNet[j].from)
             {
                countSink++;
                break;
             }
           }
           
           if(countSink == 0)
           {
               foundSink++;
               sink = flowNet[i].to;
           }
        }
    
        if(foundSink != 1)
        {
            throw std::runtime_error(NOT_ONE_SINK);
        }
    }
    return 0;
}

int edgeExists(const std::vector<Edge>& flow, unsigned int from, unsigned int to)
{
    for(unsigned int i = 0; i < flow.size(); i++)
    {
        if(flow[i].from == from && flow[i].to == to)
        {
            return i;
        }
    }
    return -1;
}

bool bfs(const std::vector<Edge>& resG,
    unsigned int numberOfVertices, std::vector<unsigned int>& paths, 
    const std::vector<Edge>& flowNet)
{
    //paths.clear();
    unsigned int countSink = 1;
    unsigned int foundSink = 0;  
    unsigned int sink = -1;
    
    for(unsigned int i =0; i < paths.size(); i++)
    {
        paths[i] = numberOfVertices;
    }

        for(unsigned int i=0; i < flowNet.size(); i++)
        {
           countSink = 0;

            if(flowNet[i].to == sink)
            {
                continue;
            }


           for(unsigned int j=0; j < flowNet.size(); j++)
           {
             if(flowNet[i].to == flowNet[j].from)
             {
                countSink++;
                break;
             }
           }
           
           if(countSink == 0)
           {
               foundSink++;
               sink = flowNet[i].to;
           }
        }
    std::vector<bool> discovered(numberOfVertices);

    for(unsigned int i = 0; i < resG.size(); i++)
    {
        discovered[i] = false;
    }

        unsigned int count = 0;
        unsigned int found = 0;
        unsigned int source = -1;

        for(unsigned int i=0; i < flowNet.size(); i++)
        {
             count = 0;
            if(flowNet[i].from == source)
            {
                continue;
            }

           for(unsigned int j=0; j < flowNet.size(); j++)
           {
             if(flowNet[i].from == flowNet[j].to)
             {
                count++;
                break;
             }
           }

           if(count == 0)
           {
               found++;
               source = flowNet[i].from;
           }
        }

    std::queue<int> q;
    q.push(source);
    unsigned int current = 0;
    paths[source] = numberOfVertices;
    discovered[source] = true;
    while(!q.empty())
    {
        current = q.front();
        q.pop();

        for(unsigned int i=0; i < resG.size(); i++)
        {
            if(resG[i].from == current)
            {
                if(discovered[resG[i].to] == false && resG[i].weight > 0)
                {
                    if(sink == resG[i].to)
                    {
                        paths[resG[i].to] = current;
                        return true;
                    }
                    discovered[resG[i].to] = true;
                    q.push(resG[i].to);
                    paths[resG[i].to] = current;

                }
            }
        }
    
    }

    return false;
}


std::vector<Edge> solveNetworkFlow(
    const std::vector<Edge>& flowNetwork,
    unsigned numVertices)
{
    check(flowNetwork, numVertices);
    std::vector<Edge> residual(flowNetwork.size());
    std::vector<Edge> flow(flowNetwork.size());
    std::vector<unsigned int> paths(numVertices);

    unsigned int countSink = 1;
    unsigned int foundSink = 0; 
    unsigned int sink = -1;

        for(unsigned int i=0; i < flowNetwork.size(); i++)
        {
           countSink = 0;

            if(flowNetwork[i].to == sink)
            {
                continue;
            }


           for(unsigned int j=0; j < flowNetwork.size(); j++)
           {
             if(flowNetwork[i].to == flowNetwork[j].from)
             {
                countSink++;
                break;
             }
           }
           
           if(countSink == 0)
           {
               foundSink++;
               sink = flowNetwork[i].to;
           }
        }

    for(unsigned int i=0; i < flowNetwork.size(); i++) //flow
    {
        flow[i].from = flowNetwork[i].from;
        flow[i].to = flowNetwork[i].to;
        flow[i].weight = 0;
    }

    for(unsigned int i=0; i < flowNetwork.size(); i++) // residual
    {
        residual[i].from = flowNetwork[i].from;
        residual[i].to = flowNetwork[i].to;
        residual[i].weight = flowNetwork[i].weight;
    }
         unsigned int count = 0;
        unsigned int found = 0;
        unsigned int source = -1;

        for(unsigned int i=0; i < flowNetwork.size(); i++)
        {
             count = 0;
            if(flowNetwork[i].from == source)
            {
                continue;
            }

           for(unsigned int j=0; j < flowNetwork.size(); j++)
           {
             if(flowNetwork[i].from == flowNetwork[j].to)
             {
                count++;
                break;
             }
           }

           if(count == 0)
           {
               found++;
               source = flowNetwork[i].from;
           }
        }

    unsigned int  bottleneck = 0;

    while(bfs(residual, numVertices, paths, flowNetwork))
    {
        bottleneck = INT_MAX;
        for(unsigned int i = sink; i != source; i = paths[i])
        {
            for(unsigned int j = 0; j < residual.size(); j++)
            {
                if(residual[j].to == i && residual[j].from == paths[i])
                {
                    if(bottleneck > residual[j].weight)
                    {
                        bottleneck = residual[j].weight; 
                    }
                }
            }
        }
        for(unsigned int i = sink; i != source; i = paths[i])
        {
            for(unsigned int j = 0; j < residual.size(); j++)
            {
                if(residual[j].to == i && residual[j].from == paths[i])
                {
                    int flowEdge = edgeExists(flow, residual[j].from, residual[j].to);
                    if(flowEdge != -1)
                    {
                      flow[flowEdge].weight += bottleneck;  
                    } 
                    int edge = edgeExists(flow, residual[j].to, residual[j].from);
                    if(edge != -1)
                    {
                       flow[edge].weight -= bottleneck; 
                    }            
                }
                
            }
        }

        for(unsigned int i = sink; i != source; i = paths[i])
        {
          for(unsigned int j = 0; j < residual.size(); j++)
            {
                if(residual[j].to == i && residual[j].from == paths[i])
                {
                    residual[j].weight = residual[j].weight - bottleneck;

                    int edge = 0;
                    edge = edgeExists(residual, residual[j].to, residual[j].from);
                    if(edge == -1)
                    {
                        residual.push_back({residual[j].to, residual[j].from,
                        bottleneck});
                    }
                    else
                    {
                        residual[edge].weight += bottleneck; 
                    }
                }
            }
        }
    }
    
    return flow;
}

void assignCourses(
    std::vector<Instructor>& instructors,
    const std::vector<std::string>& courses)
{
    std::vector<Edge> courseGraph;
    unsigned int numVertice = instructors.size() + courses.size() + 2;
    unsigned int source = numVertice - 2;
    unsigned int sink = numVertice - 1;

    for(unsigned int i =0; i < instructors.size(); i++)
    {
        courseGraph.push_back({source, i, instructors[i].maxCourses}); 
    }

    for(unsigned int i =0; i < instructors.size(); i++)
    {   
        for(unsigned int j = 0; j < instructors[i].preferences.size(); j++)
        {
            for(unsigned int k = 0; k < courses.size(); k++)
           {
                if(instructors[i].preferences[j] == courses[k])
                {
                    courseGraph.push_back({i, k+(unsigned)instructors.size(), 1});  
                }
           }
        }  
    }

    for(unsigned int i = instructors.size(); i < courses.size()+instructors.size(); i++)
    {
        courseGraph.push_back({i, sink, 1}); // classes to sink
    }

    std::vector<Edge> maxFlow = solveNetworkFlow(courseGraph, numVertice);
    unsigned int courseNum = 0;

    for(unsigned int i = 0; i < instructors.size(); i++)
    {
        for(unsigned int j = 0; j < maxFlow.size(); j++)
        {
            if(maxFlow[j].from == i && maxFlow[j].weight == 1)
            {
                courseNum = maxFlow[j].to - instructors.size();
                instructors[i].assignedCourses.push_back(courses[courseNum]);
                 
            }
        }
    }

}
