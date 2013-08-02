#ifndef _UNORDER_TREE_H
#define _UNORDER_TREE_H
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
			int index_num;
			void index(){
				node* parent_point = parent;
				node* child_point = this;
				while(parent_point){
					if(&*parent_point->left == child_point)
						++parent_point->index_num;
					child_point=parent_point;
					parent_point = parent_point->parent;
				}
			}

			void un_index(){
				node* parent_point = parent;
				node* child_point = this;
				while(parent_point){
					if(&*parent_point->left == child_point)
						--parent_point->index_num;
					child_point=parent_point;
					parent_point = parent_point->parent;
				}
			}
			node(node* pa,T* p = nullptr):parent(pa),ptr(p),index_num(0){ }
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
			void rotate_left(node_ptr& root_ptr);
			void rotate_right(node_ptr& root_ptr);
			void dump(int level);
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
			bool operator ==(iterator it){return point==it.point;}
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

		iterator insert(iterator itr, const T& val);
		void remove(iterator itr);

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

#endif
