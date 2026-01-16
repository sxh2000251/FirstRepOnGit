#include <iostream>
#include <random>

import uuid_tool;

int main()
{
    uuid_tool::uuid empty;
    std::cout << std::boolalpha << empty.is_nil() << std::endl;
    std::cout << std::noboolalpha << empty.is_nil() << std::endl;

    {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size>{};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);
        uuid_tool::uuid_random_generator gen{generator};

        const uuid_tool::uuid id = gen();
        std::cout << id << std::endl;
    }

    {
        std::random_device rd;
        auto seed_data = std::array<int, 6>{};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::ranlux48_base generator(seq);

        uuid_tool::basic_uuid_random_generator<std::ranlux48_base> dgen(&generator);
        auto id1 = dgen();
        std::cout << id1 << std::endl;
    }

    {
        uuid_tool::uuid_name_generator dgen(
            uuid_tool::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43").value());
        auto id1 = dgen("john");
        auto id2 = dgen("jane");
        auto id4 = dgen(L"jane");
        std::cout << uuid_tool::to_string(id1) << std::endl;
        std::cout << uuid_tool::to_string(id2) << std::endl;
        std::cout << uuid_tool::to_string(id4) << std::endl;
    }

    {
        auto str = "47183823-2574-4bfd-b411-99ed177d3e43";
        auto guid = uuid_tool::uuid::from_string(str).value();
        auto h2 = std::hash<uuid_tool::uuid>{};
        std::cout << h2(guid) << std::endl;
    }
}
