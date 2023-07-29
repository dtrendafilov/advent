#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <functional>
#include <unordered_map>
#include <cctype>
#include <cstdlib>
#include <cassert>

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
};

struct Unknown
{
};

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



auto solve_rhs(std::plus<int64_t>, int64_t result, int64_t rhs)
{
    return result - rhs;
}

auto solve_rhs(std::minus<int64_t>, int64_t result, int64_t rhs)
{
    return result + rhs;
}

auto solve_rhs(std::multiplies<int64_t>, int64_t result, int64_t rhs)
{
    return result / rhs;
}

auto solve_rhs(std::divides<int64_t>, int64_t result, int64_t rhs)
{
    return result * rhs;
}

auto solve_lhs(std::plus<int64_t>, int64_t result, int64_t lhs)
{
    return result - lhs;
}

auto solve_lhs(std::minus<int64_t>, int64_t result, int64_t lhs)
{
    return lhs - result;
}

auto solve_lhs(std::multiplies<int64_t>, int64_t result, int64_t lhs)
{
    return result / lhs;
}

auto solve_lhs(std::divides<int64_t>, int64_t result, int64_t lhs)
{
    return lhs / result;
}


struct FindXVisitor
{
    int64_t operator()(int64_t) const
    {
        return _result;
    }

    template <typename Op>
    int64_t operator()(const MathJob<Op>& job) const
    {
        auto& lhs = (*_monkeys)[job.left];
        auto& rhs = (*_monkeys)[job.right];

        assert(rhs.index() == 0 || lhs.index() == 0);

        // TODO: replace "humn" with Unknown to simplify this
        if (lhs.index() != 0 || job.left == "humn")
        {
            auto result = solve_rhs(Op{}, _result, std::get<0>(rhs));
            return std::visit(FindXVisitor{_monkeys, result}, lhs);
        }
        if (rhs.index() != 0 || job.right == "humn")
        {
            auto result = solve_lhs(Op{}, _result, std::get<0>(lhs));
            return std::visit(FindXVisitor{_monkeys, result}, rhs);
        }
        return 0;
    }

    int64_t operator()(const Equal&) const
    {
        assert(false && "not reached");
        return 0;
    }

    Monkeys* _monkeys;
    int64_t _result;
};


struct SolveForVisitor
{
    Job operator()(int64_t number) const
    {
        return number;
    }

    template <typename Op>
    Job operator()(const MathJob<Op>& job) const
    {
        auto& lhs = (*_monkeys)[job.left];
        auto& rhs = (*_monkeys)[job.right];

        if (job.left != _unknown)
        {
            lhs = std::visit(*this, lhs);
        }

        if (job.right != _unknown)
        {
            rhs = std::visit(*this, rhs);
        }

        // TODO: replace "humn" with Unknown to simplify this
        if (job.left != _unknown && job.right != _unknown
                && lhs.index() == 0 && rhs.index() == 0)
        {
            return Op{}(std::get<0>(lhs), std::get<0>(rhs));
        }
        return job;
    }

    Job operator()(const Equal& job) const
    {
        auto& lhs = (*_monkeys)[job.left];
        auto& rhs = (*_monkeys)[job.right];

        lhs = std::visit(*this, lhs);

        rhs = std::visit(*this, rhs);

        assert(lhs.index() == 0 || rhs.index() == 0);
        assert(lhs.index() != 0 || rhs.index() != 0);

        if (lhs.index() != 0)
        {
            auto result = std::get<0>(rhs);
            return std::visit(FindXVisitor{_monkeys, result}, lhs);
        }
        if (rhs.index() != 0)
        {
            auto result = std::get<0>(lhs);
            return std::visit(FindXVisitor{_monkeys, result}, rhs);
        }
        return job;
    }

    Monkeys* _monkeys;
    std::string _unknown;
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
        auto& root = monkeys["root"];
        root = std::visit(ToEqual{}, root);
        assert(root.index() == 5);
        SolveForVisitor solveFor{&monkeys, "humn"};
        auto result = std::visit(solveFor, root);
        assert(result.index() == 0);
        std::cout << "shout: " << std::get<0>(result) << std::endl;
    }

    return 0;
}
