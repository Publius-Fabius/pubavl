
/** AVLTree Node */
class AVLNode {
        /** Construct a new AVLNode for an AVLTree. */
        constructor(key, value) 
        {
                this.key = key;
                this.value = value;
                this.left = null;
                this.right = null;
                this.height = 1;
        }

        /** Get the node's height, or 0 if the node is null. */
        static height(node) 
        {
                return node === null ? 0 : node.height;
        }

        /** Recompute the node's height. */
        updateHeight() 
        {
                this.height = 1 + Math.max(
                        AVLNode.height(this.left), AVLNode.height(this.right));
                return this;
        }

        /** Get this node's balance factor. */
        balanceFactor() 
        {
                return AVLNode.height(this.right) - AVLNode.height(this.left);
        }

        /** Rotate this node to the right. */
        rotateRight() 
        {
                const x = this.left;
                const tmp = x.right;
                x.right = this;
                this.left = tmp;
                this.updateHeight();
                x.updateHeight();
                return x;
        }

        /** Rotate this node to the left. */
        rotateLeft() 
        {
                const y = this.right;
                const tmp = y.left;
                y.left = this;
                this.right = tmp;
                this.updateHeight();
                y.updateHeight();
                return y;
        }

        /** Rebalance this node. */
        rebalance() 
        {
                const balance = this.balanceFactor();
                if(balance > 1) {
                        if(this.right.balanceFactor() < 0) {
                                this.right = this.right.rotateRight();
                                return this.rotateLeft();
                        } else {
                                return this.rotateLeft();
                        }
                } else if(balance < -1) {
                        if(this.left.balanceFactor() > 0) {
                                this.left = this.left.rotateLeft();
                                return this.rotateRight();
                        } else {
                                return this.rotateRight();
                        }
                } else {
                        return this;
                }
        }

        /** Add a <key,value> pair to the tree. */
        static add(node, key, val, result) 
        {
                if(node === null) {
                        result.node = new AVLNode(key, val);
                        return result.node;
                } else if(key < node.key) {
                        node.left = AVLNode.add(node.left, key, val, result);
                        return node.updateHeight().rebalance();
                } else if(node.key < key) {
                        node.right = AVLNode.add(node.right, key, val, result);
                        return node.updateHeight().rebalance();
                } else {
                        return node;
                }
        }

        /** Get the node with the given key. */
        static get(node, key) 
        {
                for(;;) {
                        if(node === null) {
                                return null;
                        } else if(key < node.key) {
                                node = node.left;
                        } else if(node.key < key) {
                                node = node.right;
                        } else {
                                return node;
                        }
                }
        }

        /** Helper function for AVLNode.pluck. */
        static pluck_min(node, result) 
        {
                if(node.left !== null) {
                        node.left = AVLNode.pluck_min(node.left, result);
                        return node.updateHeight().rebalance();
                } else {
                        result.node = node;
                        return node.right
                }
        }

        /** Pluck the node from the tree. */
        static pluck(node) 
        {
                if(node.left === null) {
                        return node.right;
                } else if(node.right === null) {
                        return node.left;
                } else {
                        const result = { };
                        node.right = AVLNode.pluck_min(node.right, result);
                        const succ = result.node;
                        succ.left = node.left;
                        succ.right = node.right;
                        return succ.updateHeight().rebalance();
                }
        }

        /** Remove an entry with the given key. */
        static remove(node, key, result) 
        {
                if(node === null) {
                        return null;
                } else if(key < node.key) {
                        node.left = AVLNode.remove(node.left, key, result);
                        return node.updateHeight().rebalance();
                } else if(node.key < key) {
                        node.right = AVLNode.remove(node.right, key, result);
                        return node.updateHeight().rebalance();
                } else {
                        result.node = node;
                        return AVLNode.pluck(node);
                }
        }

        /** Get the minimum value starting from the given node. */
        static min(node) 
        {
                if(node === null) {
                        return null;
                } else for(;;node = node.left) {
                        if(node.left === null) {
                                return node;
                        }
                }
        }

        /** Get the maximum value starting from the given node. */
        static max(node) 
        {
                if(node === null) {
                        return null;
                } else for(;;node = node.right) {
                        if(node.right === null) {
                                return node;
                        }
                }
        }

        /** Remove the last node from the tree. */
        static removeMin(node, result) 
        {
                if(node.left !== null) {
                        node.left = AVLNode.removeMin(node.left, result);
                        return node.updateHeight().rebalance();
                } else {
                        result.node = node;
                        return AVLNode.pluck(node);
                }
        }

        /** Remove the first node from the tree. */
        static removeMax(node, result) 
        {
                if(node.right !== null) {
                        node.right = AVLNode.removeMax(node.right, result);
                        return node.updateHeight().rebalance();
                } else {
                        result.node = node;
                        return AVLNode.pluck(node);
                }
        }

        /** An iterator from the first element greater or equal to key. */ 
        static upper(node, key) 
        {
                const iter = new AVLIterator(null);
                for(;;) {
                        if(key < node.key) {
                                iter.stack.push(node);
                                if(node.left === null) {
                                        return iter;
                                } else {
                                        node = node.left;
                                }
                        } else if(node.key < key) {
                                if(node.right === null) {
                                        return iter;
                                } else {
                                        node = node.right;
                                }
                        } else {
                                iter.stack.push(node);
                                return iter;
                        }
                }
        }

        /** An iterator from the first element less than or equal to key. */
        static lower(node, key) 
        {
                const iter = new AVLIteratorReversed(null);
                for(;;) {
                        if(key < node.key) {
                                if(node.left === null) {
                                        return iter;
                                } else {
                                        node = node.left;
                                }
                        } else if(node.key < key) {
                                iter.stack.push(node);
                                if(node.right === null) {
                                        return iter;
                                } else {
                                        node = node.right;
                                }
                        } else {
                                iter.stack.push(node);
                                return iter;
                        }
                }
        }
}

/** Balanced Search Tree */
class AVLTree {

        /** Create a new AVLTree */
        constructor() 
        {
                this.root = null;
                this.size = 0;
        }

        /** Add a new entry unless it already exists. */
        add(key, value) 
        {
                const result = { };
                this.root = AVLNode.add(this.root, key, value, result);
                if(result.node !== undefined) {
                        this.size += 1;
                        return result.node;
                } else {
                        return null;
                }
        }

        /** Remove the entry with the given key. */
        remove(key, result) 
        {
                const arg = { };
                this.root = AVLNode.remove(this.root, key, arg);
                if(arg.node !== undefined) {
                        this.size -= 1;
                        if(result) {
                                result.key = arg.node.key;
                                result.value = arg.node.value;
                        }
                        return this;
                } else {
                        return null;
                }
        }

        /** Get the key's node if it exists. */
        get(key) 
        {
                return AVLNode.get(this.root, key);
        }

        /** Get the tree's minimum node */
        min() 
        {
                return AVLNode.min(this.root);
        }

        /** Get the tree's maximum node. */
        max() 
        {
                return AVLNode.max(this.root);
        }

        /** Remove the first entry. */
        removeMin(result) 
        {
                if(this.root === null) {
                        return null;
                } else {
                        const arg = { };
                        this.root = AVLNode.removeMin(this.root, arg);
                        if(arg.node !== undefined) {
                                this.size -= 1;
                                if(result) {
                                        result.key = arg.node.key;
                                        result.value = arg.node.value;
                                }
                                return this;
                        } else {
                                return null;
                        }
                }
        }

        /** Remove the last entry. */
        removeMax(result) 
        {
                if(this.root === null) {
                        return null;
                } else {
                        const arg = { };
                        this.root = AVLNode.removeMax(this.root, arg);
                        if(arg.node !== undefined) {
                                this.size -= 1;
                                if(result) {
                                        result.key = arg.node.key;
                                        result.value = arg.node.value;
                                }
                                return this;
                        } else {
                                return null;
                        }
                }
        }

        /** Get an iterator that traverses the tree in reversed order. */
        reversed() 
        {
                return new AVLIteratorReversed(this.root);
        }

        /** Get the default iterator. */
        [Symbol.iterator]() 
        {
                return new AVLIterator(this.root);
        }

        /** Get an iterator from the first element equal to or larger than key. */
        upper(key) 
        {
                return AVLNode.upper(this.root, key);
        }

        /** Get a reverse iterator from the first element equal to or less than key. */
        lower(key) 
        {
                return AVLNode.lower(this.root, key);
        }
}

class AVLIterator {
        constructor(root) 
        {
                this.stack = [];
                for(;root; root = root.left) {
                        this.stack.push(root);
                }
        }

        next() 
        {
                const node = this.stack.pop();
                if(node === undefined) {
                        return { done: true };
                } else if(node.right !== null) {
                        for(let n = node.right; n !== null; n = n.left) {
                                this.stack.push(n);
                        }
                } 
                return { value: node, done: false };
        }

        [Symbol.iterator]() 
        {
                return this;
        }
};

class AVLIteratorReversed {
        constructor(root) 
        {
                this.stack = [];
                for(;root; root = root.right) {
                        this.stack.push(root);
                }
        }

        next() 
        {
                const node = this.stack.pop();
                if(node === undefined) {
                        return { done: true };
                } else if(node.left !== null) {
                        for(let n = node.left; n !== null; n = n.right) {
                                this.stack.push(n);
                        }
                } 
                return { value: node, done: false };
        }

        [Symbol.iterator]() 
        {
                return this;
        }
};