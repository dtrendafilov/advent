#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <functional>
#include <unordered_map>
#include <cctype>
#include <cstdlib>

template <typename Op>
struct MathJob
{
    std::string left;
    std::string right;
};

typedef MathJob<std::plus<int64_t>> Add;
typedef MathJob<std::minus<int64_t>> Substract;
typedef MathJob<std::multiplies<int64_t>> Multiply;
typedef MathJob<std::divides<int64_t>> Divide;

struct Equal
{
    std::string left;
    std::string right;
} ;

typedef std::variant<int64_t,
        Add, Substract, Multiply, Divide,
        Equal> Job;

typedef std::unordered_map<std::string, Job> Monkeys;


Monkeys read(const char* input_name)
{
    Monkeys result;

    std::ifstream input(input_name);
    std::string line;
    while (std::getline(input, line))
    {
        std::string name = line.substr(0, 4);
        Job job;
        if (std::isdigit(line[6]))
        {
            auto number = std::atoi(line.substr(6).c_str());
            job = number;
        }
        else
        {
            auto left = line.substr(6, 4);
            auto right = line.substr(13, 4);
            auto op = line[11];
            switch (op)
            {
                case '+': job = Add{left, right}; break;
                case '-': job = Substract{left, right}; break;
                case '*': job = Multiply{left, right}; break;
                case '/': job = Divide{left, right}; break;
            }
        }
        result[std::move(name)] = std::move(job);
    }

    return result;
}

struct Shout
{
    int64_t operator()(int number) const
    {
        return number;
    }

    template <typename Op>
    int64_t operator()(const MathJob<Op>& job) const
    {
        auto left = std::visit(*this, (*_monkeys)[job.left]);
        auto right = std::visit(*this, (*_monkeys)[job.right]);
        return Op{}(left, right);
    }

    int64_t operator()(const Equal& job) const
    {
        auto left = std::visit(*this, (*_monkeys)[job.left]);
        auto right = std::visit(*this, (*_monkeys)[job.right]);
        return left == right;
    }
    Monkeys* _monkeys;
};



struct SolveForHumn
{
    int64_t operator()(int number) const
    {
        return number;
    }
};

struct ToEqual
{
    Job operator()(auto job) const
    {
        return job;
    }
    template <typename Op>
    Job operator()(const MathJob<Op>& job) const
    {
        return Equal{job.left, job.right};
    }
};

int main(int argc, const char* argv[])
{
    static_assert(sizeof(Job) == 8 + 2 * sizeof(std::string), "waste detected");
    auto input = (argc > 1)? argv[1] : "input.txt";
    auto monkeys = read(input);

    // Part 1
    {
        Shout shout{&monkeys};
        auto root = std::visit(shout, monkeys["root"]);
        std::cout << root << std::endl;
    }

    // Part 2
    {
        auto root = monkeys["root"];
        monkeys["root"] = std::visit(ToEqual{}, root);
    }

    return 0;
}
