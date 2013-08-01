#include <iostream>
#include <memory>
template<typename T>
class unorder_tree{
	protected:
	public:
		struct node;
		typedef std::unique_ptr<node> node_ptr;

		struct node{
			typedef std::unique_ptr<T> segment_ptr;
			enum {
				black=0,
				red=1
			} color;
			node_ptr left,right;
			node* parent;
			segment_ptr ptr;
			node(node* pa,T* p = nullptr):parent(pa),ptr(p){ }
			node* grandparent(){
				return parent->parent;
			}
			node* uncle(){
				if(parent==&*grandparent()->left)
					return &*grandparent()->right;
				else
					return &*grandparent()->left;
			}
			node_ptr* sibling(node_ptr* child_ptr_point){
				if(child_ptr_point==&left)
					return &right;
				return &left;
			}
			void rotate_left(node_ptr& root_ptr){
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

			void rotate_right(node_ptr& root_ptr){
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
			void dump(int level){
				if(left) left->dump(level+1);
				int c=level;
				while(c--)
					std::cerr<<' ';
				std::cerr<<level<<'\t'<<color<<'\t';
				if(ptr)
					std::cerr<<*ptr<<std::endl;
				else
					std::cerr<<std::endl;
				if(right) right->dump(level+1);
			}
		};
		struct iterator_traits {
			typedef std::bidirectional_iterator_tag iterator_category;
			typedef T			value_type;
			typedef std::ptrdiff_t		difference_type;
			typedef T*		pointer;
			typedef T&		reference;
		};
		struct iterator:public iterator_traits{
			node* point;
			iterator operator ++ (){
				node* search_point = point;
				if(search_point->right){
					search_point = &*search_point->right;
					while(search_point->left)
						search_point = &*search_point->left;
					point = search_point;
				}
				else while(search_point->parent){
					if(&*search_point->parent->left==search_point){
						point = search_point->parent;
						break;
					}
					search_point = search_point->parent;
				}
				return *this;
			}

			iterator operator -- (){
				node* search_point = point;
				if(search_point->left){
					search_point = &*search_point->left;
					while(search_point->right)
						search_point = &*search_point->right;
					point = search_point;
				}
				else while(search_point->parent){
					if(&*search_point->parent->right==search_point){
						point = search_point->parent;
						break;
					}
					search_point = search_point->parent;
				}
				return *this;
			}
			bool operator !=(iterator it){return point!=it.point;}
			T& operator *(){return *point->ptr;}
			T* operator -> (){return &*point->ptr;}
		};

		iterator begin(){
			iterator itr;
			itr.point=&*root;
			while(itr.point->left)
				itr.point=&*itr.point->left;
			return itr;
		}
		iterator end(){
			return finish;
		}


		iterator insert(iterator itr, const T& val){
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


		void remove(iterator itr){
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
			search_point = &*pick_ptr;//may be nullptr
			node_ptr* child_ptr_point;
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

		void push_back(const T& val){
			insert(finish,val);
		}
		template<typename InputIterator,typename = std::_RequireInputIter<InputIterator>>
			unorder_tree(InputIterator first,InputIterator last):unorder_tree(){
				while(first!=last){
					push_back(*first);
					++first;
				}
			}

		unorder_tree():root(new node{nullptr}){
			//start.point = &*root;
			finish.point = &*root;
			root->color=node::black;
		}
		void dump(){
			root->dump(0);
		}
	private:
		void rb(node* pnode);
		void drb(node_ptr* child_ptr_point,node* parent_point);
		node_ptr root;
		//iterator start;
		iterator finish;
};

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
		if(*child_ptr_point && *child_ptr_point->color){//replace a red node to black
			*child_ptr_point->color=node::black;
			break;
		}
		else if(parent_point){//case black black
			node_ptr* sibling_ptr_point = parent_point->sibling(child_ptr_point);//FIXME null child ptr
			if(*sibling_ptr_point && (!*sibling_ptr_point->left||*sibling_ptr_point->left->color==node::black)
					&& (!*sibling_ptr_point->right||*sibling_ptr_point->right->color==node::black) ){
				if(parent_point->color){//case 4
					if(*sibling_ptr_point->color==node::black){
						parent_point->color = node::black;
						*sibling_ptr_point->color = node::red;
						break;
					}
				}
				else{//case 2 3
					if(*sibling_ptr_point->color ){
						parent_point->color = node::red;
						*sibling_ptr_point->color = node::black;
						if(*child_ptr_point==parent_point->left)
							parent_point->rotate_left(root);
						else
							parent_point->rotate_right(root);
						continue;
					}
					if(*sibling_ptr_point->color==node::black ){
						*sibling_ptr_point->color = node::red;
						//*child_ptr_point = parent_point;
						continue;
					}
				}
			}
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
	for(auto& l:texts)
		cout<<l<<endl;
	texts.dump();
	//texts.push_back(line{"test"});

}
