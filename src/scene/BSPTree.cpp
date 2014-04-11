#include "BSPTree.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

BSPTreeNode::BSPTreeNode(BSPTree* t, double sx, double sy, double sz, double sw, double sd, double sh)
	:bsptree(t), x(sx), y(sy), z(sz), w(sw), d(sd), h(sh) {
		memset(children, 0, sizeof(children));
		memset(sibling, 0, sizeof(sibling));
		type = BSP_LEAF;
		box.min = vec3f(x, y, z);
		box.max = vec3f(x + w, y + d, z + h);
}

BSPTreeNode::~BSPTreeNode() {
	int i;
	if(type == BSP_INODE) {
		for(i = 0; i < 8; i++) {
			delete children[i];
		}
	}
}

bool BSPTreeNode::isIn(vec3f p) const {
	return box.intersects(p);
}

const BSPTreeNode* BSPTreeNode::locate(vec3f p, vec3f direct) const {
	int h_s, w_s, d_s;
	if(type == BSP_LEAF) {
		return this;
	}
	else {
		if(p[0] >= x + w / 2 + RAY_EPSILON) {
			w_s = 1;
		}
		else if(p[0] <= x + w / 2 - RAY_EPSILON) {
			w_s = 0;
		}
		else if(direct[0] > -RAY_EPSILON) {
			w_s = 1;
		}
		else {
			w_s = 0;
		}
		
		if(p[1] >= y + d / 2 + RAY_EPSILON) {
			d_s = 2;
		}
		else if(p[1] <= y + d / 2 - RAY_EPSILON) {
			d_s = 0;
		}
		else if(direct[1] > -RAY_EPSILON) {
			d_s = 2;
		}
		else {
			d_s = 0;
		}
		
		if(p[2] >= z + h / 2 + RAY_EPSILON) {
			h_s = 4;
		}
		else if(p[2] <= z + h / 2 - RAY_EPSILON) {
			h_s = 0;
		}
		else if(direct[2] > -RAY_EPSILON) {
			h_s = 4;
		}
		else {
			h_s = 0;
		}

		return children[w_s + d_s + h_s]->locate(p, direct);
	}
}

const BSPTreeNode* BSPTreeNode::locate(vec3f p, vec3f direct, int face) const {
	int w_s, d_s, h_s;
	if(type == BSP_LEAF) {
		return this;
	}
	if(face == 0) {
		w_s = 0;
	}
	else if(face == 3) {
		w_s = 1;
	}
	else {
		if(p[0] >= x + w / 2 + RAY_EPSILON) {
			w_s = 1;
		}
		else if(p[0] <= x + w / 2 - RAY_EPSILON) {
			w_s = 0;
		}
		else if(direct[0] > -RAY_EPSILON) {
			w_s = 1;
		}
		else {
			w_s = 0;
		}
	}
	if(face == 2) {
		d_s = 2;
	}
	else if(face == 5) {
		d_s = 0;
	}
	else {
		
		if(p[1] >= y + d / 2 + RAY_EPSILON) {
			d_s = 2;
		}
		else if(p[1] <= y + d / 2 - RAY_EPSILON) {
			d_s = 0;
		}
		else if(direct[1] > -RAY_EPSILON) {
			d_s = 2;
		}
		else {
			d_s = 0;
		}
	}

	if(face == 4) {
		h_s = 4;
	}
	else if(face == 1) {
		h_s = 0;
	}
	else {
		if(p[2] >= z + h / 2 + RAY_EPSILON) {
			h_s = 4;
		}
		else if(p[2] <= z + h / 2 - RAY_EPSILON) {
			h_s = 0;
		}
		else if(direct[2] > -RAY_EPSILON) {
			h_s = 4;
		}
		else {
			h_s = 0;
		}
	}
	return children[w_s + d_s + h_s]->locate(p, direct, face);
}

void BSPTreeNode::build(list<Geometry*>& objs, int depth) {
	int i, j;
	typedef list<Geometry*>::const_iterator citr;
	if(depth <= 0 || objs.size() <= 1) {
		type = BSP_LEAF;
		objects = objs;
	}
	else {
		type = BSP_INODE;
		for(i = 0; i < 8; i++) {
			children[i] = new BSPTreeNode(bsptree,
							x + double(i & 1) * w / 2, y + double( (i > 1 && i < 4) || i > 5) * d / 2, z + double(i >= 4) * h / 2,
							w / 2, d / 2, h / 2);
			if(i & 1) {
				children[i]->sibling[0] = sibling[0];
			}
			else {
				children[i]->sibling[3] = sibling[3];
			}

			if( i >= 4 ) {
				children[i]->sibling[1] = sibling[1];
			}
			else {
				children[i]->sibling[4] = sibling[4];
			}

			if( i <= 1 || (i >= 4 && i <= 5)) {
				children[i]->sibling[2] = sibling[2];
			}
			else {
				children[i]->sibling[5] = sibling[5];
			}
		}
		//construct sibling
		for(i = 0; i < 4; i++) {
			//LR
			children[i << 1]->sibling[0] = children[(i << 1) + 1];
			children[(i << 1) + 1]->sibling[3] = children[i << 1];
			//UD
			children[i]->sibling[1] = children[i + 4];
			children[i + 4]->sibling[4] = children[i];
			//FB
			j = ((i << 1) & 4) + (i & 1);
			children[j]->sibling[5] = children[j + 2];
			children[j + 2]->sibling[2] = children[j];
		}
		
		list<Geometry*> new_q;
		for(i = 0; i < 8; i++) {
			new_q.clear();
			//Compute the objects that intersects and then build it recursively
			for(citr it = objs.begin(); it != objs.end(); ++it) {
				BoundingBox b = (*it)->getBoundingBox();
				if(b.intersects(children[i]->box)) {
					new_q.push_back(*it);
				}
			}
			children[i]->build(new_q, depth - 1);
		}
	}
}

bool BSPTreeNode::intersect(const ray& r, isect& i, const BSPTreeNode*& next) const {
	bool have_one = false;
	typedef list<Geometry*>::const_iterator iter;
	iter j;
	isect cur;
	double tmin, tmax, tt;
	box.intersect(r, tmin, tmax);
	tt = tmax + 1;
	for( j = objects.begin(); j != objects.end(); ++j ) {
		if( (*j)->intersect( r, cur ) && cur.t < tmax ) {
			if( !have_one || (cur.t < i.t) ) {
				i = cur;
				have_one = true;
			}
		}
	}
	if(!have_one) {
		//Find next
		/*
		assert(box.intersect(r, tmin, tmax));
		vec3f end = r.at(tmax);
		assert(tmax > -RAY_EPSILON);
		if(end[0] >= x + w - RAY_EPSILON) {
			//Right out
			next = sibling[0]?sibling[0]->locate(end, r.getDirection(), 0):NULL;
		}
		else if(end[0] <= x + RAY_EPSILON) {
			//Left out
			next = sibling[3]?sibling[3]->locate(end, r.getDirection(), 3):NULL;
		}
		else if(end[1] >= y + d - RAY_EPSILON) {
			//Back out
			next = sibling[5]?sibling[5]->locate(end, r.getDirection(), 5):NULL;
		}
		else if(end[1] <= y + RAY_EPSILON) {
			//Front out
			next = sibling[2]?sibling[2]->locate(end, r.getDirection(), 2):NULL;
		}
		else if(end[2] >= z + h - RAY_EPSILON) {
			//Up out
			next = sibling[1]?sibling[1]->locate(end, r.getDirection(), 1):NULL;
		}
		else {
			//Down out
			next = sibling[4]?sibling[4]->locate(end, r.getDirection(), 4):NULL;
		}*/
		next = NULL;
		ray rr(r.at(tmax), r.getDirection());
		for(int m = 0; m < 6; m++) {
			if(sibling[m] && sibling[m]->box.intersect(rr, tmin, tmax) && tmin > -RAY_EPSILON && tmin < tt) {
				next = sibling[m]->locate(rr.at(tmin), r.getDirection(), m);
			}
		}
	}
	return have_one;
}

BSPTree::BSPTree(Scene* scene)
:s(scene), root(NULL){
}

BSPTree::~BSPTree() {
	delete root;
}

void BSPTree::build() {
	BoundingBox b = s->getBound();
	root = new BSPTreeNode(this, b.min[0], b.min[1], b.min[2], b.max[0] - b.min[0], b.max[1] - b.min[1], b.max[2] - b.min[2]);
	root->build(s->boundedobjects, BSPTREE_MAX_DEPTH);
}

bool BSPTree::intersect(const ray& r, isect &i) const {
	const BSPTreeNode *startp;
	double tmin, tmax;
	ray rr(r);
	i.obj = NULL;
	if(!root->box.intersect(r, tmin, tmax) || tmax <= -RAY_EPSILON) {
		return false;
	}
	if(!root->isIn(r.getPosition())) {
		root->box.intersect(r, tmin, tmax);
		rr = ray(r.at(tmin), r.getDirection());
	}
	else {
		tmin = 0;
	}
	startp = root->locate(rr.getPosition(), rr.getDirection());
	while(!startp->intersect(rr, i, startp)) {
		if(!startp) {
			return false;
		}
	}
	i.t += tmin;
	return true;
}