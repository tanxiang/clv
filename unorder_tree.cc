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
			node* get_node(){return point;}
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

		void rb(node* pnode){
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

	iterator insert(iterator itr, const T& val){
		node* search_point = itr.get_node();
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
	node_ptr root;
	//iterator start;
	iterator finish;
};

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
