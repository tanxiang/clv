#include "unorder_tree.h"
#include <iostream>
template<typename T>
void unorder_tree<T>::node::dump(int level){
	if(left) left->dump(level+1);
	int c=level;
	while(c--)
	std::cout<<' ';
	std::cout<<level<<'\t'<<color<<'\t';
	if(ptr)
		std::cout<<*ptr<<std::endl;
	else
		std::cout<<std::endl;
	if(right) right->dump(level+1);
}

template<typename T>
void unorder_tree<T>::node::rotate_left(node_ptr& root_ptr){
	if(!right)
		std::cerr<<"error rotate_left right==null\n";
	node_ptr new_root = std::move(right);
	if(new_root->left){
		right = std::move(new_root->left);				
		right->parent = this;
	}
	node_ptr* hander;
	if(parent)
		hander = this == &*parent->left ? &parent->left : &parent->right;
	else
		hander = &root_ptr;
	new_root->parent = parent;
	new_root->left = std::move(*hander);
	parent = &*new_root;
	*hander=std::move(new_root);
}

template<typename T>
void  unorder_tree<T>::node::rotate_right(node_ptr& root_ptr){
	if(!left)
		std::cerr<<"error rotate_right left==null\n";
	node_ptr new_root = std::move(left);
	if(new_root->right){
		left = std::move(new_root->right);
		left->parent = this;
	}
	node_ptr* hander;
	if(parent)
		hander = this == &*parent->left ? &parent->left : &parent->right;
	else
		hander = &root_ptr;
	new_root->parent = parent;
	new_root->right = std::move(*hander);
	parent = &*new_root;
	*hander=std::move(new_root);
}

template<typename T>
typename unorder_tree<T>::iterator unorder_tree<T>::insert(iterator itr, const T& val){
	node* search_point = itr.point;
	if(!search_point->left){
		search_point->left =node_ptr{new node{search_point,new T{val}}};
		rb(&*search_point->left);
	}
	else{
		search_point=&*search_point->left;
		while(search_point->right)
			search_point=&*search_point->right;
		search_point->right =node_ptr{new node{search_point,new T{val}}};
		rb(&*search_point->right);
	}
	return --itr;
}

template<typename T>
void unorder_tree<T>::remove(iterator itr){
	if(itr==finish)
		return;
	node* search_point = itr.point;
	if(search_point->left && search_point->right){
		search_point=&*search_point->right;
		while(search_point->left)
			search_point=&*search_point->left;
		itr.point->ptr = std::move(search_point->ptr);
	}
	node_ptr pick_ptr = search_point->right ? std::move(search_point->right) :
		search_point->left ? std::move(search_point->left) : nullptr;

	if(pick_ptr)
		pick_ptr->parent = search_point->parent;//nullptr if root
	node* parent_point = search_point->parent;
	node_ptr* child_ptr_point;
	//std::cerr<<&**child_ptr_point<<";"<<search_point<<":"<<parent_point<<":"<<&*parent_point->left<<"\n";
	if(!parent_point)
		child_ptr_point=&root;
	else if(search_point==&*parent_point->left)
		child_ptr_point = &parent_point->left;
	else
		child_ptr_point = &parent_point->right;
	std::swap(pick_ptr,*child_ptr_point);
	if(pick_ptr->color==node::black){//delete a black node need fix b
		drb(child_ptr_point,parent_point);
	}
}

template<typename T>
void unorder_tree<T>::rb(node* pnode){
	pnode->color = node::red;
	//rb case
	do{
		if(!pnode->parent){
			pnode->color = node::black;
			break;
		}
		if(pnode->parent->color){
			node* uncle = pnode->uncle();
			if(uncle && uncle->color){//case 3
				pnode->parent->color = node::black;
				uncle->color=node::black;
				pnode->grandparent()->color = node::red;
				pnode = pnode->grandparent();
				continue;
			}
			else{//case 4,5
				if(pnode == &*pnode->parent->left && pnode->parent == &*pnode->grandparent()->right){
					pnode->parent->rotate_right(root);
					pnode = &*pnode->right;
				}
				else if(pnode == &*pnode->parent->right && pnode->parent == &*pnode->grandparent()->left){
					pnode->parent->rotate_left(root);
					pnode = &*pnode->left;
				}
				if(pnode == &*pnode->parent->right){
					pnode->grandparent()->rotate_left(root);
					pnode->parent->color = node::black;
					pnode->parent->left->color = node::red;
				}else{
					pnode->grandparent()->rotate_right(root);
					pnode->parent->color = node::black;
					pnode->parent->right->color = node::red;
				}
			}
		}
		break;
	}while(true);
}

template<typename T>
void unorder_tree<T>::drb(node_ptr* child_ptr_point,node* parent_point){
	do{
		if(*child_ptr_point && (*child_ptr_point)->color){//replace a red node to black
			(*child_ptr_point)->color=node::black;
			break;
		}
		else if(parent_point){
			node_ptr* sibling_ptr_point = parent_point->sibling(child_ptr_point);
			std::cerr<<"case run\n";
			if((!(*sibling_ptr_point)->left||(*sibling_ptr_point)->left->color==node::black)//FIXME *sibling_ptr_point will be null unique_ptr??
					&& (!(*sibling_ptr_point)->right||(*sibling_ptr_point)->right->color==node::black) ){
				if(parent_point->color){//case 4
					std::cerr<<"case 4\n";
					if((*sibling_ptr_point)->color==node::black){
						parent_point->color = node::black;
						(*sibling_ptr_point)->color = node::red;
						break;
					}
				}
				else{
					if((*sibling_ptr_point)->color ){//case 2
						std::cerr<<"case 2\n";
						parent_point->color = node::red;
						(*sibling_ptr_point)->color = node::black;
						if(*child_ptr_point==parent_point->left)
							parent_point->rotate_left(root);
						else
							parent_point->rotate_right(root);
						continue;
					}
					if((*sibling_ptr_point)->color==node::black ){//case 3
						std::cerr<<"case 3\n";
						(*sibling_ptr_point)->color = node::red;
						if(!parent_point->parent)//parent_point is root down
							break;
						if(parent_point==&*parent_point->parent->left)
							child_ptr_point = &parent_point->parent->left;
						else
							child_ptr_point = &parent_point->parent->right;
						parent_point = parent_point->parent;
						continue;
					}
				}
			}
			if(child_ptr_point==&parent_point->left){
				if(!(*sibling_ptr_point)->right || (*sibling_ptr_point)->right->color==node::black){//(*sibling_ptr_point)->left will be red case 5
					std::cerr<<"case l5>\n";
					(*sibling_ptr_point)->left->color=node::black;
					(*sibling_ptr_point)->color=node::red;
					node_ptr* root_ptr_point = &(*sibling_ptr_point)->left;
					(*sibling_ptr_point)->rotate_right(root);
					sibling_ptr_point = root_ptr_point;
				}
				std::cerr<<"case l6\n";
				(*sibling_ptr_point)->color = parent_point->color;
				parent_point->color = node::black;
				parent_point->rotate_left(root);
			}
			else{
				if(!(*sibling_ptr_point)->left || (*sibling_ptr_point)->left->color==node::black){//(*sibling_ptr_point)->right will be red case 5
					std::cerr<<"case l5>\n";
					(*sibling_ptr_point)->right->color=node::black;
					(*sibling_ptr_point)->color=node::red;
					node_ptr* root_ptr_point = &(*sibling_ptr_point)->right;
					(*sibling_ptr_point)->rotate_left(root);
					sibling_ptr_point = root_ptr_point;
				}
				std::cerr<<"case r6\n";
				(*sibling_ptr_point)->color = parent_point->color;
				parent_point->color = node::black;
				parent_point->rotate_right(root);
			}
			break;
		}
		else
			break;
	}while(true);
}


class line :public std::string 
{
	public:
		friend std::istream & operator>>(std::istream & is, line& l){
			return std::getline(is, l);
		}
		virtual ~line(){
			//std::cerr<<"line free\n";
		}
};

#include <iterator>     // std::istream_iterator
#include <algorithm>
using namespace std;
int main(){
	unorder_tree<line> texts{istream_iterator<line>{cin},istream_iterator<line>{}};
	//copy(texts.begin(),texts.end(),ostream_iterator<line>{cout});
	//for(auto& l:texts)
	//cout<<l<<endl;
	texts.dump();
	cout<<"---------------------------\n";
	int i=6;
	while(--i){
		auto itr = texts.begin();
		texts.remove(itr);
	}
	texts.dump();
	//texts.push_back(line{"test"});

}
