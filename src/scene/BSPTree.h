#ifndef __BSPTREE_H__
#define __BSPTREE_H__

#include "scene.h"
#include "ray.h"
#include <list>
using std::list;

class BSPTree;

/*
 * children:
 * Top
 * +---+---+
 * | 6 | 7 |
 * +---+---+
 * | 4 | 5 |
 * +---+---+
 * Bottom
 * +---+---+
 * | 2 | 3 |
 * +---+---+
 * | 0 | 1 |
 * +---+---+
 */

/*
 * siblings:
 * z               y
 * ^   +-------+  /\
 * |  /       /|  /
 * | /   1   / | /
 *  /       /  |
 * +-------+ 0 |
 * |       |  /
 * |   2   | /
 * |       |/
 * +-------+  -->x
 * 0<-->3
 * 1<-->4
 * 2<-->5
 */
enum BSPTreeNodeType {
	BSP_LEAF = 0,
	BSP_INODE
};

#define BSPTREE_MAX_DEPTH (7)

class BSPTreeNode {
	public:
		BSPTreeNode(BSPTree* t, double sx, double sy, double sz, double sw, double sd, double sh);
		~BSPTreeNode();

		bool intersect(const ray &r, isect& i, const BSPTreeNode*& next) const;
		const BSPTreeNode* locate(vec3f p, vec3f d) const;
		const BSPTreeNode* locate(vec3f p, vec3f d, int face) const;
		void build(list<Geometry*>& objs, int depth);

		bool isIn(vec3f p) const;

		friend class BSPTree;
	protected:
		double x, y, z, w, d, h;

		BoundingBox box;
		BSPTree* bsptree;
		BSPTreeNode *sibling[6];
		BSPTreeNode *children[8];
		list<Geometry*> objects;
		enum BSPTreeNodeType type;

};

class BSPTree {
	public:
		BSPTree(Scene *scene);
		~BSPTree();

		void build();
		bool intersect(const ray &r, isect& i) const;

	protected:
		BSPTreeNode* root;
		Scene* s;
};

#endif
