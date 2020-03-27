#include <iostream>
#include <set>
#include <assert.h>
#include <memory>
#include <vector>
#include <list>
#include <fstream>
#ifndef ONLINE_JUDGE
#include <boost/algorithm/string/trim.hpp>
#include <sstream>
#endif
#include <iomanip>
#include <cassert>
#include <memory>

enum class Instruction
{
    PARENT,
    FIRST_CHILD,
    NEXT_SIBLING,
    PREVIOUS_SIBLING
};

class Node
    : public std::enable_shared_from_this<Node>
{
public:
    typedef std::shared_ptr<Node> shared_ptr_t;

public:
    static shared_ptr_t create(const std::string & value) { return shared_ptr_t(new Node(value)); }

private:
    Node(const std::string & value)
        : value_(value)
    {

    }
    Node(const Node & node); // = delete

public:
    std::string value() const { return value_; }

    shared_ptr_t parent() const { return parent_; }
    
    void add_child(shared_ptr_t that)
    {
        children_.push_back(that);
        that->parent_ = shared_from_this();
        that->iter_ = --children_.end();
    }

    shared_ptr_t first_child() const
    {
        if (children_.size())
        {
            return children_.front();
        }

        return shared_ptr_t();
    }

    shared_ptr_t next_sibling() const
    {
        if (parent_ && iter_ != --parent_->children_.end())
        {
            auto next_iter = iter_;
            next_iter++;
            return *next_iter;
        }

        return shared_ptr_t();
    }

    shared_ptr_t previous_sibling() const
    {
        if (parent_ && iter_ != parent_->children_.begin())
        {
            auto previous_iter = iter_;
            previous_iter--;
            return *previous_iter;
        }

        return shared_ptr_t();
    }

private:
    void set_parent(shared_ptr_t that) { parent_ = that; }

private:
    std::string value_;
    shared_ptr_t parent_;
    std::list<shared_ptr_t> children_;
    std::list<shared_ptr_t>::iterator iter_;
};

std::string parse_node_value(const std::string & str)
{
    auto begin_pos = str.find("'");
    auto end_pos = str.rfind("'");

    if (begin_pos == str.npos || end_pos == str.npos || begin_pos == end_pos)
        throw std::runtime_error("error format");

    return str.substr(begin_pos + 1, end_pos - begin_pos - 1);
}

size_t read_line_as_number(std::istream & in)
{
    std::string str;
    if (!getline(in, str))
        throw std::runtime_error("error format");

    size_t result(0);
    if (!std::sscanf(str.c_str(), "%d", &result))
        throw std::runtime_error("error format");
    return result;
}

std::string read_line(std::istream & in)
{
    std::string str;
    if (!getline(in, str))
        throw std::runtime_error("error format");
    return str;
}

Node::shared_ptr_t parse_html_document(std::istream & in, size_t size)
{
    // create root
    Node::shared_ptr_t root = Node::create(parse_node_value(read_line(in)));

    // create followed
    Node::shared_ptr_t current = root;
    for (size_t i = 1; i < size; ++i)
    {
        std::string line = read_line(in);

        if (line == "</n>")
        {
            current = current->parent();
        }
        else
        {
            assert(current);
            auto child = Node::create(parse_node_value(line));
            current->add_child(child);
            current = child;
        }
    }
    assert(!current);
    return root;
}

std::vector<Instruction> parse_instructions(std::istream & in)
{
    std::vector<Instruction> result;

    size_t instructions_count = read_line_as_number(in);   
    for (size_t i = 0; i < instructions_count; ++i)
    {
        std::string instruction = read_line(in);

        if (instruction == "parent")
        {
            result.push_back(Instruction::PARENT);
        }
        else if (instruction == "first_child")
        {
            result.push_back(Instruction::FIRST_CHILD);
        }
        else if (instruction == "next_sibling")
        {
            result.push_back(Instruction::NEXT_SIBLING);
        }
        else if (instruction == "previous_sibling")
        {
            result.push_back(Instruction::PREVIOUS_SIBLING);
        }
    }

    return result;
}

std::vector<Node::shared_ptr_t> calculate_passed_nodes(Node::shared_ptr_t starts, const std::vector<Instruction> & instructions)
{
    std::vector<Node::shared_ptr_t> result;

    auto current = starts;
    for (auto instruction : instructions)
    {
        if (instruction == Instruction::PARENT)
        {
            if (current->parent())
                current = current->parent();
        }
        else if (instruction == Instruction::FIRST_CHILD)
        {
            if (current->first_child())
                current = current->first_child();
        }
        else if (instruction == Instruction::NEXT_SIBLING)
        {
            if (current->next_sibling())
                current = current->next_sibling();
        }
        else if (instruction == Instruction::PREVIOUS_SIBLING)
        {
            if (current->previous_sibling())
                current = current->previous_sibling();
        }

        result.push_back(current);
    }

    return result;
}


void run(std::istream & in, std::ostream & out)
{
    int case_no = 0;
    std::string line;
    while (1)
    {
        size_t html_document_size = read_line_as_number(in);
        if (html_document_size)
        {
            auto root = parse_html_document(in, html_document_size);
            auto instructions = parse_instructions(in);
            auto passed_nodes = calculate_passed_nodes(root, instructions);

            out << "Case " << case_no + 1 << ":" << std::endl;
            for (auto node : passed_nodes)
            {
                out << node->value() << std::endl;
            }

            // case end with blank line
            std::string blank_line = read_line(in);
            assert(blank_line.empty());
        }
        else
        {
            break;
        }
       
        case_no++;
    }
    
}

#ifdef ONLINE_JUDGE

int main(int argc, char** argv)
{
    run(std::cin, std::cout);
    return 0;
}

#else

int main(int argc, char** argv)
{
    std::ifstream stream_in("input.txt");
    std::stringstream stream_out;
    run(stream_in, stream_out);

    std::string my_answer = stream_out.str();
    std::ifstream t("output.txt");
    std::string expected_answer((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());

    BOOST_ASSERT(boost::trim_right_copy(my_answer) == boost::trim_right_copy(expected_answer));
    return 0;
}
#endif
