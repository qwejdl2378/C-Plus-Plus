#include <iostream>

using namespace std;

// 红黑树节点结构体定义
struct node
{
	int key;       // 节点键值
	node *parent;  // 指向父节点的指针
	char color;    // 节点颜色：'r' 代表红色，'b' 代表黑色
	node *left;    // 指向左子节点的指针
	node *right;   // 指向右子节点的指针
};

// 红黑树类定义
class RBtree
{
	node *root;    // 树根节点指针
	node *q;       // 辅助指针
public:
	RBtree()
	{
		q = NULL;
		root = NULL;
	}
	void insert();           // 插入节点方法
	void insertfix(node *);  // 插入后的平衡修正方法
	void leftrotate(node *); // 左旋转操作
	void rightrotate(node *);// 右旋转操作
	void del();              // 删除节点方法
	node* successor(node *); // 查找后继节点（用于删除）
	void delfix(node *);     // 删除后的平衡修正方法
	void disp();             // 显示树的入口方法
	void display(node *);    // 递归显示树结构
	void search();           // 查找节点方法
};

// 插入新节点的主逻辑
void RBtree::insert()
{
	int z;
	cout << "\nEnter key of the node to be inserted: ";
	cin >> z;
	node *p, *q;
	node *t = new node;
	t->key = z;
	t->left = NULL;
	t->right = NULL;
	t->color = 'r'; // 新插入节点默认为红色
	p = root;
	q = NULL;

	// 1. 标准二叉搜索树 (BST) 的插入过程
	if (root == NULL)
	{
		root = t;
		t->parent = NULL;
	}
	else
	{
		while (p != NULL)
		{
			q = p;
			if (p->key < t->key)
				p = p->right;
			else
				p = p->left;
		}
		t->parent = q;
		if (q->key < t->key)
			q->right = t;
		else
			q->left = t;
	}

	// 2. 插入红色节点后，可能会破坏红黑树性质，调用修正算法
	insertfix(t);
}

// 插入后的红黑树性质修正算法
void RBtree::insertfix(node *t)
{
	node *u; // 叔叔节点指针
	if (root == t)
	{
		t->color = 'b'; // 规则1：根节点必须为黑色
		return;
	}
	// 当父节点存在且为红色时，违反了“红色节点不能有红色子节点”的性质，需要进行调整
	while (t->parent != NULL && t->parent->color == 'r')
	{
		node *g = t->parent->parent; // 祖父节点
		// 情况 A：父节点是祖父节点的左子节点
		if (g->left == t->parent)
		{
			if (g->right != NULL)
			{
				u = g->right; // 叔叔节点
				// 情况 A1：叔叔节点也是红色，只需重新着色并向上回溯
				if (u->color == 'r')
				{
					t->parent->color = 'b';
					u->color = 'b';
					g->color = 'r';
					t = g; // 指针向上回溯到祖父节点，继续检查
				}
			}
			else
			{
				// 情况 A2：当前节点是父节点的右子节点，需先进行左旋转换为 A3
				if (t->parent->right == t)
				{
					t = t->parent;
					leftrotate(t);
				}
				// 情况 A3：当前节点是父节点的左子节点，且叔叔为黑色。对祖父节点右旋并重新着色
				t->parent->color = 'b';
				g->color = 'r';
				rightrotate(g);
			}
		}
		// 情况 B：父节点是祖父节点的右子节点（对称操作）
		else
		{
			if (g->left != NULL)
			{
				u = g->left; // 叔叔节点
				// 情况 B1：叔叔节点是红色，重新着色
				if (u->color == 'r')
				{
					t->parent->color = 'b';
					u->color = 'b';
					g->color = 'r';
					t = g;
				}
			}
			else
			{
				// 情况 B2：当前节点是父节点的左子节点，先进行右旋转换为 B3
				if (t->parent->left == t)
				{
					t = t->parent;
					rightrotate(t);
				}
				// 情况 B3：当前节点是父节点的右子节点，且叔叔为黑色。对祖父节点左旋并重新着色
				t->parent->color = 'b';
				g->color = 'r';
				leftrotate(g);
			}
		}
		root->color = 'b'; // 始终确保根节点为黑色
	}
}

// 删除节点的主逻辑
void RBtree::del()
{
	if (root == NULL)
	{
		cout << "\nEmpty Tree.";
		return;
	}
	int x;
	cout << "\nEnter the key of the node to be deleted: ";
	cin >> x;
	node *p;
	p = root;
	node *y = NULL;
	node *q = NULL;
	int found = 0;

	// 1. 在树中查找要删除的节点
	while (p != NULL && found == 0)
	{
		if (p->key == x)
			found = 1;
		if (found == 0)
		{
			if (p->key < x)
				p = p->right;
			else
				p = p->left;
		}
	}
	if (found == 0)
	{
		cout << "\nElement Not Found.";
		return;
	}
	else
	{
		cout << "\nDeleted Element: " << p->key;
		cout << "\nColour: ";
		if (p->color == 'b')
			cout << "Black\n";
		else
			cout << "Red\n";

		// 打印节点的关系信息
		if (p->parent != NULL)
			cout << "\nParent: " << p->parent->key;
		else
			cout << "\nThere is no parent of the node.  ";
		if (p->right != NULL)
			cout << "\nRight Child: " << p->right->key;
		else
			cout << "\nThere is no right child of the node.  ";
		if (p->left != NULL)
			cout << "\nLeft Child: " << p->left->key;
		else
			cout << "\nThere is no left child of the node.  ";
		cout << "\nNode Deleted.";

		// 2. 确定真正被删除的物理节点 y
		// 如果 p 没有或只有一个子节点，则直接删除 p；如果有两个子节点，则删除其前驱/后继节点 y，然后用 y 的值覆盖 p。
		if (p->left == NULL || p->right == NULL)
			y = p;
		else
			y = successor(p); // 查找后继节点

		// 3. 确定 y 的唯一非空子节点 q (若存在)
		if (y->left != NULL)
			q = y->left;
		else
		{
			if (y->right != NULL)
				q = y->right;
			else
				q = NULL;
		}

		// 4. 将子节点 q 与 y 的父节点进行链接
		if (q != NULL)
			q->parent = y->parent;
		if (y->parent == NULL)
			root = q;
		else
		{
			if (y == y->parent->left)
				y->parent->left = q;
			else
				y->parent->right = q;
		}

		// 5. 如果真正被删除的节点 y 不是 p，则把 y 的数据拷贝到 p 中
		if (y != p)
		{
			p->color = y->color;
			p->key = y->key;
		}

		// 6. 如果删除的节点 y 是黑色，会造成黑高度不平衡，必须调用 delfix 进行修正
		if (y->color == 'b')
			delfix(q);
	}
}

// 删除后的红黑树性质修正算法（恢复黑节点平衡）
void RBtree::delfix(node *p)
{
	node *s; // 兄弟节点指针
	while (p != root && p->color == 'b')
	{
		// 情况 1：当前节点 p 是其父节点的左子节点
		if (p->parent->left == p)
		{
			s = p->parent->right; // 兄弟节点
			// 情况 1.1：兄弟节点是红色。通过变色和左旋，转换为兄弟节点为黑色的情况
			if (s->color == 'r')
			{
				s->color = 'b';
				p->parent->color = 'r';
				leftrotate(p->parent);
				s = p->parent->right;
			}
			// 情况 1.2：兄弟节点的两个子节点都是黑色。只需将兄弟节点染红，将黑冲突上移给父节点
			if (s->right->color == 'b' && s->left->color == 'b')
			{
				s->color = 'r';
				p = p->parent; // 指针上移
			}
			else
			{
				// 情况 1.3：兄弟节点的右节点是黑色，左节点是红色。通过右旋兄弟节点，转换为情况 1.4
				if (s->right->color == 'b')
				{
					s->left->color = 'b';
					s->color = 'r';
					rightrotate(s);
					s = p->parent->right;
				}
				// 情况 1.4：兄弟节点的右节点是红色。重新着色并对父节点左旋，达到黑高度平衡
				s->color = p->parent->color;
				p->parent->color = 'b';
				s->right->color = 'b';
				leftrotate(p->parent);
				p = root; // 修正完毕，退出循环
			}
		}
		// 情况 2：当前节点 p 是其父节点的右子节点（对称操作）
		else
		{
			s = p->parent->left; // 兄弟节点
			if (s->color == 'r')
			{
				s->color = 'b';
				p->parent->color = 'r';
				rightrotate(p->parent);
				s = p->parent->left;
			}
			if (s->left->color == 'b' && s->right->color == 'b')
			{
				s->color = 'r';
				p = p->parent;
			}
			else
			{
				if (s->left->color == 'b')
				{
					s->right->color = 'b';
					s->color = 'r';
					leftrotate(s);
					s = p->parent->left;
				}
				s->color = p->parent->color;
				p->parent->color = 'b';
				s->left->color = 'b';
				rightrotate(p->parent);
				p = root;
			}
		}
		p->color = 'b';
		root->color = 'b';
	}
}

// 左旋转操作（用于平衡调节）
void RBtree::leftrotate(node *p)
{
	if (p->right == NULL)
		return;
	else
	{
		node *y = p->right;
		if (y->left != NULL)
		{
			p->right = y->left;
			y->left->parent = p;
		}
		else
			p->right = NULL;
		if (p->parent != NULL)
			y->parent = p->parent;
		if (p->parent == NULL)
			root = y;
		else
		{
			if (p == p->parent->left)
				p->parent->left = y;
			else
				p->parent->right = y;
		}
		y->left = p;
		p->parent = y;
	}
}

// 右旋转操作（用于平衡调节）
void RBtree::rightrotate(node *p)
{
	if (p->left == NULL)
		return;
	else
	{
		node *y = p->left;
		if (y->right != NULL)
		{
			p->left = y->right;
			y->right->parent = p;
		}
		else
			p->left = NULL;
		if (p->parent != NULL)
			y->parent = p->parent;
		if (p->parent == NULL)
			root = y;
		else
		{
			if (p == p->parent->left)
				p->parent->left = y;
			else
				p->parent->right = y;
		}
		y->right = p;
		p->parent = y;
	}
}

// 寻找中序前驱/后继节点
node* RBtree::successor(node *p)
{
	node *y = NULL;
	if (p->left != NULL)
	{
		y = p->left;
		while (y->right != NULL)
			y = y->right;
	}
	else
	{
		y = p->right;
		while (y->left != NULL)
			y = y->left;
	}
	return y;
}

// 打印树的外部入口
void RBtree::disp()
{
	display(root);
}

// 递归遍历并层级化打印整棵红黑树，展示节点键值、颜色、左右子节点及父节点关系
void RBtree::display(node *p)
{
	if (root == NULL)
	{
		cout << "\nEmpty Tree.";
		return;
	}
	if (p != NULL)
	{
		cout << "\n\t NODE: ";
		cout << "\n Key: " << p->key;
		cout << "\n Colour: ";
		if (p->color == 'b')
			cout << "Black";
		else
			cout << "Red";
		if (p->parent != NULL)
			cout << "\n Parent: " << p->parent->key;
		else
			cout << "\n There is no parent of the node.  ";
		if (p->right != NULL)
			cout << "\n Right Child: " << p->right->key;
		else
			cout << "\n There is no right child of the node.  ";
		if (p->left != NULL)
			cout << "\n Left Child: " << p->left->key;
		else
			cout << "\n There is no left child of the node.  ";
		cout << endl;
		if (p->left)
		{
			cout << "\n\nLeft:\n";
			display(p->left);
		}
		if (p->right)
		{
			cout << "\n\nRight:\n";
			display(p->right);
		}
	}
}

// 查找指定键值的节点
void RBtree::search()
{
	if (root == NULL)
	{
		cout << "\nEmpty Tree\n";
		return;
	}
	int x;
	cout << "\n Enter key of the node to be searched: ";
	cin >> x;
	node *p = root;
	int found = 0;
	while (p != NULL && found == 0)
	{
		if (p->key == x)
			found = 1;
		if (found == 0)
		{
			if (p->key < x)
				p = p->right;
			else
				p = p->left;
		}
	}
	if (found == 0)
		cout << "\nElement Not Found.";
	else
	{
		cout << "\n\t FOUND NODE: ";
		cout << "\n Key: " << p->key;
		cout << "\n Colour: ";
		if (p->color == 'b')
			cout << "Black";
		else
			cout << "Red";
		if (p->parent != NULL)
			cout << "\n Parent: " << p->parent->key;
		else
			cout << "\n There is no parent of the node.  ";
		if (p->right != NULL)
			cout << "\n Right Child: " << p->right->key;
		else
			cout << "\n There is no right child of the node.  ";
		if (p->left != NULL)
			cout << "\n Left Child: " << p->left->key;
		else
			cout << "\n There is no left child of the node.  ";
		cout << endl;
	}
}

// 交互式主函数，供测试红黑树的所有功能
int main()
{
	int ch, y = 0;
	RBtree obj;
	do
	{
		cout << "\n\t RED BLACK TREE ";
		cout << "\n 1. Insert in the tree ";
		cout << "\n 2. Delete a node from the tree";
		cout << "\n 3. Search for an element in the tree";
		cout << "\n 4. Display the tree ";
		cout << "\n 5. Exit ";
		cout << "\nEnter Your Choice: ";
		cin >> ch;
		switch (ch)
		{
		case 1: obj.insert();
			cout << "\nNode Inserted.\n";
			break;
		case 2: obj.del();
			break;
		case 3: obj.search();
			break;
		case 4: obj.disp();
			break;
		case 5: y = 1;
			break;
		default: cout << "\nEnter a Valid Choice.";
		}
		cout << endl;

	} while (y != 1);
	return 1;
}
