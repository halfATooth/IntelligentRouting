#include "central-controller.h"

namespace ns3
{

CentralController::CentralController(NetBuilder nb)
{
    netBuilder = nb;
    m_nodes = nb.getNodes();
    m_adj = nb.getAdj();
    isAdjReady = true;
}

void
CentralController::AddTopologyInfo(std::vector<std::vector<int>> pairs, int len)
{
    // i, j, w
    for (int i = 0; i < len; i++)
    {
        m_adj[pairs[i][0]][pairs[i][1]] = pairs[i][2];
        m_adj[pairs[i][1]][pairs[i][0]] = pairs[i][2];
    }
    isAdjReady = true;
}

// void
// CentralController::CollectLinkInfo()
// {
//     // 采集链路信息
//     std::cout << "Collecting link information at " << Simulator::Now().GetSeconds() << "s"
//               << std::endl;
//     // 这里可以添加具体的信息采集代码，例如采集流量、时延、丢包率等
//     // 示例：可以通过NetDevice和PacketSink等对象来采集流量和丢包率
//     // 可以通过应用层的回调函数来记录时延

//     // 安排下一次信息采集任务
//     m_collectionEvent =
//         Simulator::Schedule(m_collectionInterval, &CentralController::CollectLinkInfo, this);
// }

void
CentralController::doUpdateRoutingTable()
{
    std::vector<Ipv4Address> nodeToIpAddress = netBuilder.getNodeToIpAddress();
    // clear
    clearRoutingTable();
    
    // init
    // does node i have a route to node j
    std::vector<std::vector<bool>> isVisit(m_nodes.GetN(),
                                           std::vector<bool>(m_nodes.GetN(), false));
    // add route
    for (int i = 0; i < m_nodes.GetN(); i++)
    {
        AddRouteFromStart(i, nodeToIpAddress, Dijkstra(i), isVisit);
    }
}

void
CentralController::AddRouteFromStart(int start,
                                     std::vector<Ipv4Address> nodeToIpAddress,
                                     std::vector<int> nextNodes,
                                     std::vector<std::vector<bool>>& isVisit)
{
    Ipv4StaticRoutingHelper staticRoutingHelper;
    Ptr<Ipv4StaticRouting> staticRouting =
        staticRoutingHelper.GetStaticRouting(m_nodes.Get(start)->GetObject<Ipv4>());
    for (int i = 0; i < nextNodes.size(); i++)
    {
        if (i == start)
        {
            continue;
        }
        Ipv4Address dst = nodeToIpAddress[i];
        int interface = netBuilder.getPort(start, nextNodes[i]);
        if (interface != -1)
        {
            staticRouting->AddHostRouteTo(dst, interface);
            isVisit[start][i] = true;
        }
    }
}

void
CentralController::clearRoutingTable()
{
    for (int i = 0; i < m_nodes.GetN(); i++)
    {
        Ipv4StaticRoutingHelper staticRoutingHelper;
        Ptr<Ipv4StaticRouting> staticRouting =
            staticRoutingHelper.GetStaticRouting(m_nodes.Get(i)->GetObject<Ipv4>());
        uint32_t num = staticRouting->GetNRoutes();
        for (uint32_t j = num-1; j > 0; j--)
        {
            std::ostringstream oss;
            oss << staticRouting->GetRoute(j).GetDestNetwork();
            std::string dst = oss.str();
            if (dst[dst.length() - 1] != '0')
            {
                staticRouting->RemoveRoute(j);
            }
        }
    }
}

void
CentralController::PrintRoutingTable()
{
    for (int i = 0; i < m_nodes.GetN(); i++)
    {
        std::cout << "node: " << i << std::endl;
        Ipv4StaticRoutingHelper staticRoutingHelper;
        Ptr<Ipv4StaticRouting> staticRouting =
            staticRoutingHelper.GetStaticRouting(m_nodes.Get(i)->GetObject<Ipv4>());
        uint32_t num = staticRouting->GetNRoutes();
        for (uint32_t j = 0; j < num; j++)
        {
            std::cout << "route: " << staticRouting->GetRoute(j) << std::endl;
        }
    }
}

void
CentralController::UpdateRoutingTable(std::string weightsData)
{
    UpdateWeights(weightsData);
    doUpdateRoutingTable();
}

void
CentralController::InitRoutingTable()
{
    doUpdateRoutingTable();
}

void
CentralController::UpdateWeights(std::string data)
{
    std::cout<<"receive:\n"<<data<<std::endl;
    int startPos = 0;
    int cursor = data.find("/", startPos);
    while (cursor != std::string::npos)
    {
        std::string link = data.substr(startPos, cursor - startPos);
        int firstSpace = link.find(' ');
        int secondSpace = link.rfind(' ');
        if (firstSpace >= secondSpace)
        {
            std::cout << "When UpdateWeights, data formation err" << std::endl;
            break;
        }
        int n0 = atoi(link.substr(0, firstSpace).c_str());
        int n1 = atoi(link.substr(firstSpace + 1, secondSpace - firstSpace - 1).c_str());
        int w = atoi(link.substr(secondSpace + 1).c_str());
        m_adj[n0][n1] = w;
        m_adj[n1][n0] = w;
        startPos = cursor + 1;
        cursor = data.find("/", startPos);
    }
}

std::string
CentralController::ConcatLinkState(int i, int j, LinkState linkState)
{
    double avgDelay = linkState.sendCount==0 ? 0 : double(linkState.delay) / linkState.sendCount;
    double avgDropRate = linkState.sendCount==0 ? 0 : double(linkState.dropCount) / linkState.sendCount;
    std::ostringstream oss;
    oss << i << " " << j << " "
        << avgDelay << " "
        << linkState.bandwidth << " "
        << avgDropRate << " "
        << linkState.throughput 
        << "\n";
    return oss.str();
}

std::string
CentralController::CollectNetInfo()
{
    std::string result;
    std::vector<std::vector<LinkState>> linkStates = netBuilder.getLinkStates();
    for (int i = 0; i < m_adj.size(); i++)
    {
        for (int j = 0; j < m_adj.size(); j++)
        {
            if (m_adj[i][j] == -1)
            {
                continue;
            }
            std::string linkInfo = ConcatLinkState(i, j, linkStates[i][j]);
            result.append(linkInfo);
        }
    }
    return result;
}

std::vector<int>
CentralController::Dijkstra(int start)
{
    if (!isAdjReady)
    {
        return std::vector<int>();
    }
    if (m_adj.empty() || m_adj.size() != m_adj[0].size())
    {
        return std::vector<int>();
    }
    int n = m_adj.size();
    if (start >= n || start < 0)
    {
        return std::vector<int>();
    }
    // nextNodes[i]: 在从start到i的最短路径中，start之后的下一跳节点的标号
    std::vector<int> nextNodes(n, 0);
    std::vector<int> distance2start(n, m_max_weight);
    std::vector<int> isCheck(n, 0);
    // init
    distance2start[start] = 0;
    isCheck[start] = 1;

    int cursor = start;
    for (int i = 0; i < n - 1; i++)
    {
        findNeighbors(start, cursor, distance2start, nextNodes, isCheck);
        // find the shortest one whitch is unchecked
        int index = -1;
        int min = m_max_weight;
        for (int j = 0; j < n; j++)
        {
            if (isCheck[j] == 1)
            {
                continue;
            }
            if (distance2start[j] < min)
            {
                min = distance2start[j];
                index = j;
            }
        }
        if (index == -1)
        {
            std::cout << "err occurred when find the shortest one" << std::endl;
            return std::vector<int>();
        }
        isCheck[index] = 1;
        cursor = index;
    }
    return nextNodes;
}

void
CentralController::findNeighbors(int start,
                                 int v,
                                 std::vector<int>& distance2start,
                                 std::vector<int>& nextNodes,
                                 std::vector<int> isCheck)
{
    for (int i = 0; i < m_adj[v].size(); i++)
    {
        if (m_adj[v][i] != -1 && isCheck[i] == 0)
        {
            int distance = distance2start[v] + m_adj[v][i];
            if (distance < distance2start[i])
            {
                distance2start[i] = distance;
                // 如果i是start的邻居，下一跳应该是i本身；
                // 否则，start之后的下一跳，i和v应该相同，因为在同一条路径上
                nextNodes[i] = v == start ? i : nextNodes[v];
            }
        }
    }
}

} // namespace ns3
