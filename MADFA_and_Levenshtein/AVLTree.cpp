#ifndef __AVL_Tree_h__   // if x.h hasn't been included yet...
#define __AVL_Tree_h__

#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
//#include "AVLTree.h"

class AVLTree
{
        private:
               class Info {
                        private:
                                int index;
                                std::string niz;
                        public:
                                Info(int ind, std::string val){
                                        index = ind;
                                        niz = val;
                                }
                                
                                std::string getNiz(){
                                        return niz;
                                }

                                int getIndex(){
                                        return index;
                                }

                };

                class Node
                {
                        private:
                                Info * data;
                                int height;
                                Node * parent;
                                Node * left_child;
                                Node * right_child;
                        public:
                                Node(int ind, std::string val)
                                {
                                        data = new Info (ind, val);                                        
                                        height = 0;
                                        parent = (Node*)0;
                                        left_child = (Node*)0;
                                        right_child = (Node*)0;
                                }
                                Info* getData()
                                {
                                        return data;
                                }

                                int getHeight()
                                {
                                        return height;
                                }
                                int updateHeight()
                                {
                                        if(left_child != 0 && right_child != 0)
                                        {
                                                if(left_child->getHeight() > right_child->getHeight())
                                                        height = left_child->getHeight() + 1;
                                                else
                                                        height = right_child->getHeight() + 1;
                                        }
                                        else if(left_child != 0)
                                                height = left_child->getHeight() + 1;
                                        else if(right_child != 0)
                                                height = right_child->getHeight() + 1;
                                        else
                                                height = 0;
                                        return height;
                                }
                                int getBalance()
                                {
                                        Node * n = this;
                                        if(n->getLeftChild() != 0 && n->getRightChild() !=0)
                                                return n->getLeftChild()->getHeight() - n->getRightChild()->getHeight();
                                        else if(n->getLeftChild() != 0)
                                                return n->getLeftChild()->getHeight() + 1;
                                        else if(n->getRightChild() != 0)
                                                return (-1) * (n->getRightChild()->getHeight() + 1);
                                        else
                                                return 0;
                                }
                                Node* getParent()
                                {
                                        return parent;
                                }
                                void removeParent()
                                {
                                        parent = (Node*)0;
                                }
                                Node* getLeftChild()
                                {
                                        return left_child;
                                }
                                Node* setLeftChild(Node* new_left)
                                {
                                        if(new_left != 0) new_left->parent = this;
                                        left_child = new_left;
                                        updateHeight();
                                        return left_child;
                                }
                                Node* getRightChild()
                                {
                                        return right_child;
                                }
                                Node* setRightChild(Node* new_right)
                                {
                                        if(new_right != 0) new_right->parent = this;
                                        right_child = new_right;
                                        updateHeight();
                                        return right_child;
                                }
                };
 
                void setRoot(Node* n)
                {
                        root = n;
                        if(root != 0) root->removeParent();
                }
 
 
                Node* findNode(std::string val)
                {
                        Node* temp = root;
                        int r;
                        while(temp != 0)
                        {
                                r = strcmp(val.c_str(), temp->getData()->getNiz().c_str());                                
                                if(r == 0)
                                        return temp;
 
                                else if(r < 0)
                                        temp = temp->getLeftChild();
 
                                else
                                        temp = temp->getRightChild();
                        }
                        return (Node*)0;
                }
                void rotateLeft(Node * n)
                {
                        Node * p = n->getParent();
                        enum {left, right} side;
                        if(p != 0)
                        {
                                if(p->getLeftChild() == n)
                                        side = left;
                                else
                                        side = right;
                        }
                        Node * temp = n->getRightChild();
                        n->setRightChild(temp->getLeftChild());
                        temp->setLeftChild(n);
                        // Now attach the subtree to the parent (or root)
                        if(p != 0)
                        {
                                if(side == left)
                                        p->setLeftChild(temp);
                                if(side == right)
                                        p->setRightChild(temp);
                        }
                        else
                        {
                                setRoot(temp);
                        }
                }
 
                void rotateRight(Node * n)
                {
                        Node * p = n->getParent();
                        enum {left, right} side;
                        if(p != 0)
                        {
                                if(p->getLeftChild() == n)
                                        side = left;
                                else
                                        side = right;
                        }
                        Node * temp = n->getLeftChild();
                        n->setLeftChild(temp->getRightChild());
                        temp->setRightChild(n);
                        // Now attach the subtree to the parent (or root)
                        if(p != 0)
                        {
                                if(side == left)
                                        p->setLeftChild(temp);
                                if(side == right)
                                        p->setRightChild(temp);
                        }
                        else
                        {
                                setRoot(temp);
                        }
                }
 
                // This function does balancing at the given node
                void balanceAtNode(Node* n)
                {                                             
                        int bal = n->getBalance();
                        if(bal > 1)
                        {
                                if(n->getLeftChild()->getBalance() < 0)
                                        rotateLeft(n->getLeftChild());
                                rotateRight(n);
                        }
                        else if(bal < -1)
                        {
                                if(n->getRightChild()->getBalance() > 0)
                                        rotateRight(n->getRightChild());
                                rotateLeft(n);
                        }
                }
 
                Node * root;

                void printNode (Node* n, std::ofstream& out) {
                        if (n != 0) {
                                if (n->getParent() != 0)
                                        out << "\t \"" << n->getData()->getNiz().substr(0, n->getData()->getNiz().length() - 1) << "\" -> \"" << n->getParent()->getData()->getNiz().substr(0, n->getParent()->getData()->getNiz().length() - 1) << "\"\n";
                                if (n->getLeftChild() != 0)
                                        out << "\t \"" << n->getData()->getNiz().substr(0, n->getData()->getNiz().length() - 1) << "\" -> \"" << n->getLeftChild()->getData()->getNiz().substr(0, n->getLeftChild()->getData()->getNiz().length() - 1) << "\"\n";
                                if (n->getRightChild() != 0)
                                        out << "\t \"" << n->getData()->getNiz().substr(0, n->getData()->getNiz().length() - 1) << "\" -> \"" << n->getRightChild()->getData()->getNiz().substr(0, n->getRightChild()->getData()->getNiz().length() - 1) << "\"\n";
                                printNode(n->getLeftChild(), out);
                                printNode(n->getRightChild(), out);
                        }
                }

                void nodeToString(Node* n, std::vector<std::string>& result) {
                    if (n != 0) {
                        result.push_back(n->getData()->getNiz());
                        nodeToString(n->getLeftChild(), result);
                        nodeToString(n->getRightChild(), result);
                    }
                }
 
        public:
                AVLTree()
                {
                        root = (Node*)0;
                }
 
                AVLTree(int ind, std::string val)
                {
                        root = new Node(ind, val);
                }
 
                AVLTree * findSubtree(std::string val)
                {
                        Node* target;
                        target = findNode(val);
 
                        if(target != 0)
                        {
                                AVLTree* subtree = new AVLTree();
                                subtree->root = target;
                                return subtree;
                        }
                        else
                                return (AVLTree*)0;
                }
                
                int findString(std::string val)
                {
                        Node* target;
                        target = findNode(val);
 
                        if(target != 0)
                        {                                
                                return target->getData()->getIndex();
                        }
                        else
                                return -1;
                }

                // Returns:
                //              true: If addition is successful
                //              false: If item already exists
                //
                bool insert(int ind, std::string val)
                {       
                        Node* added_node;
                        if(root == 0)
                        {
                                root = new Node(ind, val);
                                return true;
                        }
                        else
                        {
                                Node* temp = root;
                                int r;
                                while(true)
                                {
                                        r = strcmp(val.c_str(), temp->getData()->getNiz().c_str());
                                        if(r < 0)
                                        {
                                                if((temp->getLeftChild()) == 0)
                                                {
                                                        added_node = temp->setLeftChild(new Node(ind, val));
                                                        break;
                                                }
                                                else
                                                {
                                                        temp = temp->getLeftChild();
                                                }
 
                                        }
                                        else if(r > 0)
                                        {
                                                if((temp->getRightChild()) == 0)
                                                {
                                                        added_node = temp->setRightChild(new Node(ind, val));
                                                        break;
                                                }
                                                else
                                                {
                                                        temp = temp->getRightChild();
                                                }
 
                                        }
                                        else
                                        {
                                                return false;
                                        }
                                }
                                // The following code is for updating heights and balancing.
                                temp = added_node;
                                while(temp != 0)
                                {                                                
                                        temp->updateHeight();
                                        balanceAtNode(temp);
                                        temp = temp->getParent();
                                }
                        }
                }
 
                // Returns:
                //              true: If removal is successful
                //              false: If item is not found in the tree
                //
                bool remove(std::string val)
                {
                        if(root == 0) return false;
                        Node *replacement, *replacement_parent, *temp_node;
                        Node * to_be_removed = findNode(val);                   
                        if(to_be_removed == 0) return false;
 
                        Node * p = to_be_removed->getParent();
 
                        enum {left, right} side;
                        if(p != 0)
                        {
                                if(p->getLeftChild() == to_be_removed) side = left;
                                else side = right;
                        }
 
                        int bal = to_be_removed->getBalance();
 
                        if(to_be_removed->getLeftChild() == 0 && to_be_removed->getRightChild() == 0)
                        {
                                if(p != 0)
                                {
                                        if(side == left) p->setLeftChild((Node*)0);
                                        else p->setRightChild((Node*)0);
 
                                        delete to_be_removed;
                                        p->updateHeight();
                                        balanceAtNode(p);
                                }
                                else
                                {
                                        setRoot((Node*)0);
                                        delete to_be_removed;
                                }
 
                        }
                        else if(to_be_removed->getRightChild() == 0)
                        {
                                if(p != 0)
                                {
                                        if(side == left) p->setLeftChild(to_be_removed->getLeftChild());
                                        else p->setRightChild(to_be_removed->getLeftChild());
 
                                        delete to_be_removed;
                                        p->updateHeight();
                                        balanceAtNode(p);
                                }
                                else
                                {
                                        setRoot(to_be_removed->getLeftChild());
                                        delete to_be_removed;
                                }
                        }
 
                        else if(to_be_removed->getLeftChild() == 0)
                        {
                                if(p != 0)
                                {
                                        if(side == left) p->setLeftChild(to_be_removed->getRightChild());
                                        else p->setRightChild(to_be_removed->getRightChild());
 
                                        delete to_be_removed;
                                        p->updateHeight();
                                        balanceAtNode(p);
                                }
                                else
                                {
                                        setRoot(to_be_removed->getRightChild());
                                        delete to_be_removed;
                                }
                        }
                        else
                        {
                                if(bal > 0)
                                {
                                        if(to_be_removed->getLeftChild()->getRightChild() == 0)
                                        {
                                                replacement = to_be_removed->getLeftChild();
                                                replacement->setRightChild(to_be_removed->getRightChild());
 
                                                temp_node = replacement;
 
 
                                        }
                                        else
                                        {
                                                replacement = to_be_removed->getLeftChild()->getRightChild();
                                                while(replacement->getRightChild() != 0)
                                                {
                                                        replacement = replacement->getRightChild();
                                                }
                                                replacement_parent = replacement->getParent();
                                                replacement_parent->setRightChild(replacement->getLeftChild());
 
                                                temp_node = replacement_parent;
 
                                                replacement->setLeftChild(to_be_removed->getLeftChild());
                                                replacement->setRightChild(to_be_removed->getRightChild());
                                        }
                                }
                                else
                                {
                                        if(to_be_removed->getRightChild()->getLeftChild() == 0)
                                        {
                                                replacement = to_be_removed->getRightChild();
                                                replacement->setLeftChild(to_be_removed->getLeftChild());
 
                                                temp_node = replacement;
 
 
                                        }
                                        else
                                        {
                                                replacement = to_be_removed->getRightChild()->getLeftChild();
                                                while(replacement->getLeftChild() != 0)
                                                {
                                                        replacement = replacement->getLeftChild();
                                                }
                                                replacement_parent = replacement->getParent();
                                                replacement_parent->setLeftChild(replacement->getRightChild());
 
                                                temp_node = replacement_parent;
 
                                                replacement->setLeftChild(to_be_removed->getLeftChild());
                                                replacement->setRightChild(to_be_removed->getRightChild());
                                        }
                                }               
                                if(p != 0)
                                {
                                        if(side == left) p->setLeftChild(replacement);
                                        else p->setRightChild(replacement);
 
                                        delete to_be_removed;
                                }
                                else
                                {
                                        setRoot(replacement);
                                        delete to_be_removed;
                                }
 
                                balanceAtNode(temp_node);
                        }
 
                }
                int getHeight()
                {
                        return root->getHeight();
                }

                void printToFile(std::string file_name) {
                        if (root != 0) {
                                std::ofstream myfile;
                                myfile.open (file_name.c_str());
                                myfile << "digraph g{\n";
                                printNode(root, myfile);
                                myfile << "}";
                        }
                }

                std::vector<std::string> toString(){
                    std::vector<std::string> result;
                    if (root !=0 ) {
                        nodeToString(root, result);
                    }
                    return result;
                }
};
#endif