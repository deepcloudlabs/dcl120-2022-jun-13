// Recovering lost type information

#include <iostream>
#include <vector>

using namespace std;

// Motivation: 
// "My Component classes do not know that Composites exist. 
// They provide no help for navigating Composites, nor any help for altering the contents of a Composite. 
// This is because I would like the base class (and all its derivatives) to be reusable in contexts that do not require Composites. 
// When given a base class pointer, if I absolutely need to know whether or not it is a Composite, I will use dynamic_cast() to figure this out. 
// In those cases where dynamic_cast() is too expensive, I will use a Visitor."

class Visitor {
  public:
    virtual void visit(class Primitive *, class Component*) = 0;
    virtual void visit(class Composite *, Component*) = 0;
};

class Component {
    int value;
  public:
    Component(int val) {
        value = val;
    }
    virtual void traverse() {
        cout << value << " ";
    }
    // Having add() here sacrifices safety, but it supports transparency
    // virtual void add( Component* ) { }
    virtual void accept(Visitor &, Component*) = 0;
};

class Primitive: public Component
{
  public:
    Primitive(int val): Component(val){}
    void accept(Visitor &v, Component *c) {
        v.visit(this, c);
    }
};

class Composite: public Component {
    vector < Component * > children;
  public:
    Composite(int val): Component(val){}
    void add(Component *ele) {
        children.push_back(ele);
    }
    void accept(Visitor &v, Component *c) {
        v.visit(this, c);
    }
    void traverse() {
        Component::traverse();
        for (int i = 0; i < children.size(); i++)
          children[i]->traverse();
    }
};

class AddVisitor: public Visitor {
  public:
    void visit(Primitive *, Component*) {
         /* does not make sense */
    }
    void visit(Composite *node, Component *c) {
        node->add(c);
    }
};

int main() {
  Component *nodes[3];
  // The type of Composite* is "lost" when the object is assigned to a
  // Component*
  nodes[0] = new Composite(1);
  nodes[1] = new Composite(2);
  nodes[2] = new Composite(3);

  // If add() were in class Component, this would work
  //    nodes[0]->add( nodes[1] );
  // If it is NOT in Component, and only in Composite,  you get the error -
  //    no member function `Component::add(Component *)' defined

  // Instead of sacrificing safety, we use a Visitor to support add()
  AddVisitor addVisitor;
  nodes[0]->accept(addVisitor, nodes[1]);
  nodes[0]->accept(addVisitor, nodes[2]);
  nodes[0]->accept(addVisitor, new Primitive(4));
  nodes[1]->accept(addVisitor, new Primitive(5));
  nodes[1]->accept(addVisitor, new Primitive(6));
  nodes[2]->accept(addVisitor, new Primitive(7));

  for (int i = 0; i < 3; i++) {
    nodes[i]->traverse();
    cout << endl;
  }
return 0;
}
