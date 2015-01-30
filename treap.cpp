#include<iostream>
#include<vector>
#include<algorithm>
#include<climits>
#include<string>
#include<ctime>
#include<cstdio>
#include<cstdlib>
#define INF (1LL<<62)

using namespace std;

typedef long long ll;

/*
Verified. 
POJ 3580
AOJ 1580
etc.

POJ 3580にsubmitする際はupdate関数中の
t->sum=sum(t->lch)+sum(t->rch)+val(t);
を削除すること。
*/
template<class T>
struct Treap {

public:

  struct node_t{
    T val,mini,sum,lazy;
    node_t *lch,*rch;
    int pri,cnt;
    bool rev;
    
    node_t(T v,int p):val(v),mini(v),sum(v),pri(p),cnt(1){
      lch=rch=NULL;
      lazy=0;
      rev=false;
    }
  };

  node_t *root;
  Treap():root(NULL){}
  
private:

  T val(node_t *t){return t->val+t->lazy;}
  int count(node_t *t){return !t?0:t->cnt;}
  T sum(node_t *t){return !t?0:t->sum+t->lazy*count(t);}
  T mini(node_t *t){return !t?INF:t->mini+t->lazy;}
  
  node_t *update(node_t *t){

    if(!t)return t;

    t->cnt=count(t->lch)+count(t->rch)+1;
    t->sum=sum(t->lch)+sum(t->rch)+val(t);
    t->mini=min(min(mini(t->lch),mini(t->rch)),val(t));
    
    if(t->lazy){
      t->val+=t->lazy;
      if(t->lch)t->lch->lazy+=t->lazy;
      if(t->rch)t->rch->lazy+=t->lazy;
      t->lazy=0;
    }
    
    if(t->rev){
      swap(t->lch,t->rch);
      if(t->lch)t->lch->rev^=true;
      if(t->rch)t->rch->rev^=true;
      t->rev=false;
    }
    
    return t;
  }
  
  node_t *merge(node_t *l, node_t *r){

    l=update(l), r=update(r);
    if(!l || !r)return l?l:r;
    
    if(l->pri > r->pri){
      l->rch=merge(l->rch,r);
      return update(l);
    } else {
      r->lch=merge(l,r->lch);
      return update(r);
    }
  }
  
  //[0,k),[k,n)
  pair<node_t *,node_t *> split(node_t *t,int k){
    if(!update(t))return make_pair((node_t *)NULL,(node_t *)NULL);
    
    if(k<=count(t->lch)){
      pair<node_t *,node_t *>s=split(t->lch,k);
      t->lch=s.second;
      return make_pair(s.first,update(t));
    } else {
      pair<node_t *,node_t *>s=split(t->rch,k-count(t->lch)-1);
      t->rch=s.first;
      return make_pair(update(t),s.second);
    }
  }
  
  node_t *insert(node_t *t,int k,T val,int pri){
    pair<node_t *,node_t *>s=split(t,k);
    t=merge(s.first, new node_t(val,pri));
    t=merge(t,s.second);
    return update(t);
  }
  
  node_t *erase(node_t *t,int k){
    pair<node_t *,node_t *>s1,s2;
    s2=split(t,k+1);
    s1=split(s2.first,k);
    delete s1.second;
    return update((t=merge(s1.first,s2.second)));
  }
  
  node_t *find(node_t *t, int k){
    int c=count(t->lch);
    if(k<c)return find(t->lch, k);
    if(k==c)return t;
    return find(t->rch, k-c-1);
  }

public:

  void insert(int k,T val){ root=insert(root,k,val,rand()); }
  void erase(int k){ root=erase(root,k); }
  node_t *find(int k){ return find(root,k); }
  
  void add(int id,T val){
    node_t *a=find(id);
    T tmp=val(a);
    erase(id);
    insert(id,tmp+val);
  }

  void rangeAdd(int l,int r,T v){
    pair<node_t *,node_t *>s1,s2;
    s2=split(root,r);
    s1=split(s2.first,l);
    s1.second->lazy+=v;
    root=merge(s1.first,s1.second);
    root=merge(root,s2.second);
  }

  void update(int id,T val){
    erase(id);
    insert(id,val);
  }

  //range query [l,r), 0-origin
   
  T rangeMinimumQuery(int l,int r){
    pair<node_t *,node_t *>s1,s2;
    s2=split(root,r);
    s1=split(s2.first,l);
    T res=mini(s1.second);
    root = merge(s1.first,merge(s1.second,s2.second));
    return res;
  }
  
  T rangeSumQuery(int l,int r){
    pair<node_t *,node_t *>s1,s2;
    s2=split(root,r);
    s1=split(s2.first,l);
    T res=sum(s1.second);
    root = merge(s1.first,merge(s1.second,s2.second));
    return res;
  }
  
  /*
   reverse(1,4)
   {1,2,3,4,5} -> {1,4,3,2,5}
   */
  void reverse(int l,int r){
    pair<node_t *,node_t *>s1,s2;
    s2=split(root,r);
    s1=split(s2.first,l);
    node_t *a=s1.first,*b=s1.second,*c=s2.second;   
    b->rev^=true;
    root=merge(a,b);
    root=merge(root,c);
  }

  /*
    revolve(1,4,2)
    {1,2,3,4,5} -> {1,3,4,2,5}  
   */
  void revolve(int l,int r,int t){
    t%=(r-l);
    pair<node_t*, node_t*>a,b,c;
    c=split(root, r);
    b=split(c.first, r-t);
    a=split(b.first, l);
    root=merge(a.first, b.second);
    root=merge(root, a.second);
    root=merge(root, c.second);
  }
};
