
function shuffle(array) 
{
    return array.sort(() => Math.random() - 0.5);
}

function initKeys(count)
{
        array = [];
        for(let n = 0; n < count; ++n) {
                array[n] = n;
        }
        return shuffle(array);  
}

function addAll(tree, keys, count)
{
        for(let n = 0; n < count; ++n) {
                const k = keys[n];
                if(!tree.add(k, k)) throw new Error(); 
        }
        return tree;
}

function testAdd() 
{
        console.log("testAdd()");
        const COUNT = 1000;
        const keys = initKeys(COUNT);
        const tree = new AVLTree();
        addAll(tree, keys, COUNT);
        const iter = tree[Symbol.iterator]();
        for(let j = 0; j < COUNT; ++j) {
                const { value, done } = iter.next();
                if(done) throw new Error();
                if(value.key != j) throw new Error();
        }
}

function testRemove()
{
        console.log("testRemove()");
        const COUNT = 1000;
        const keys = initKeys(COUNT);
        const tree = new AVLTree();
        addAll(tree, keys, COUNT);
        const iter = tree[Symbol.iterator]();
        
        for(let i = 0; i < COUNT; ++i) {
                if(!tree.get(i)) throw new Error();
                tree.remove(i);
                if(tree.get(i)) throw new Error();
                const iter = tree[Symbol.iterator]();
                for(let j = i + 1; j < COUNT; ++j) {
                        const { value, done } = iter.next();
                        if(done) throw new Error();
                        if(value.key != j) throw new Error();
                }
        }
        if(tree.size != 0) throw new Error();
}

function testReversed()
{
        console.log("testReversed()");
        const COUNT = 1000;
        const keys = initKeys(COUNT);
        const tree = new AVLTree();
        addAll(tree, keys, COUNT);
        const iter = tree.reversed();
        for(let j = COUNT - 1; j > 0; --j) {
                const { value, done } = iter.next();
                if(done) throw new Error();
                if(value.key != j) throw new Error();
        }
}

function testRemoveMin()
{
        console.log("testRemoveMin()");
        const COUNT = 1000;
        const keys = initKeys(COUNT);
        const tree = new AVLTree();
        addAll(tree, keys, COUNT);
        for(let i = 0; i < COUNT; ++i) {
                const node = tree.min(); 
                if(node.key != i) throw Error();
                if(!tree.removeMin()) throw Error();
        }
        if(tree.size != 0) throw new Error();
}

function testRemoveMax()
{
        console.log("testRemoveMax()");
        const COUNT = 1000;
        const keys = initKeys(COUNT);
        const tree = new AVLTree();
        addAll(tree, keys, COUNT);
        for(let i = COUNT - 1; i >= 0; --i) {
                const node = tree.max(); 
                if(node.key != i) throw Error();
                if(!tree.removeMax()) throw Error();
        }
        if(tree.size != 0) throw new Error();
}

function testUpper()
{
        console.log("testUpper()");
        const COUNT = 1000;
        const keys = initKeys(COUNT);
        const tree = new AVLTree();
        addAll(tree, keys, COUNT);
        for(let i = COUNT - 1; i > 0; --i) {
                const iter = tree.upper(i);
                for(let j = i; j < COUNT; ++j) {
                        const { value, done } = iter.next();
                        if(done) throw new Error();
                        if(value.key != j) throw new Error();
                }
        }
}

function testLower()
{
        console.log("testLower()");
        const COUNT = 1000;
        const keys = initKeys(COUNT);
        const tree = new AVLTree();
        addAll(tree, keys, COUNT);
        for(let i = COUNT - 1; i > 0; --i) {
                const iter = tree.lower(i);
                for(let j = i; j >= 0; --j) {
                        const { value, done } = iter.next();
                        if(done) throw new Error();
                        if(value.key != j) throw new Error();
                }
        }
}

function testSuite()
{
        testAdd();
        testRemove();
        testReversed();
        testRemoveMin();
        testRemoveMax();
        testUpper();
        testLower();
}