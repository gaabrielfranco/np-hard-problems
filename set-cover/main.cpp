#include <iostream>
#include <vector>

int main()
{
    size_t n, m, element;
    char c;

    scanf("%zu %zu", &n, &m);
    std::vector<std::vector<size_t>> G;
    size_t* costs = (size_t*)malloc(m * sizeof(size_t));
    std::vector<size_t> v;

    for (size_t i = 0; i < m; i++)
    {
        while (scanf("%zu%c", &element, &c))
        {
            v.push_back(element);

            if (c == '\n')
            {
                G.push_back(v);
                v.clear();
                break;
            }
        }
    }

    for (size_t i = 0; i < m; i++)
    {
        scanf("%zu", &costs[i]);
    }

    for (size_t i = 0; i < G.size(); i++)
    {
        for (size_t j = 0; j < G[i].size(); j++)
        {
            std::cout << G[i][j] << " ";
        }
        puts("");
    }

    for (size_t i = 0; i < m; i++)
    {
        printf("%zu ", costs[i]);
    }
    puts("");

    return 0;
}