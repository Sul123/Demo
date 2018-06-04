#include <string>
#include <iostream>
#include "dynamic_graph.h"


int main() {

    int vert_num;
    std::cout << "Enter number of vertices: ";
    std::cin >> vert_num;
    DynamicGraph graph(vert_num);
    std::cout << "Available commands: \n1. add x y - add edge (x, y) to the graph\n"
        "2. remove x y - remove edge (x, y) from graph\n"
        "3. comp - print number of connected components \n"
        "4. con? x y - print are vertices x and y connected or not\n"
        "5. break - exit \n";
    while (true) {
        std::string query;
        std::cin >> query;
        if (query == "add") {
            int start, end;
            std::cin >> start >> end;
            graph.AddEdge(start, end);
        } else if (query == "remove") {
            int start, end;
            std::cin >> start >> end;
            graph.RemoveEdge(start, end);
        } else if (query == "comp") {
            std::cout << graph.GetComponentsNumber() << "\n";
        } else if (query == "break") {
            break;
        } else if (query == "con?") {
            int start, end;
            std::cin >> start >> end;
            std::cout << (graph.Connected(start, end) ? "yes" : "no") << "\n";
        }
    }

    return 0;
}
