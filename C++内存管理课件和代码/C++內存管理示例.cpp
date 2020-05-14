// author : Hou Jie (���)
// date : 2015/11/11 
// compiler : DevC++ 5.61 (MinGW with GNU 4.9.2)
//
// �f�����@�Ǻ�� E-learning video "C++�ȴ����" �Č�����ʽ.
// ԓ�n�̵����Мyԇ�����F�ڴ�.
// ÿһ��С�yԇ��Ԫ�������Mһ�� namespace ��, 
// ��˱��ָ���Ԫ�g���Ī�����.
// ÿ�� namespace �Ϸ�����ԓ��Ԫ������ #include <...> 
// ����п���������ʽ���}���� (included) ĳЩ headers,  
// �@�o���^�����ÿ�� standard headers �������ҷ��l�C�ƣ���׌�Լ��� included ����.
//
// ���ļ��õ����� GNU C++ extended allocators����������S��Ҫ����ļ��ɭh�� (IDE) ���O�� "C++11 on". 
// ���ь����P���a���b�� ���g�l���x� __GNUC__ �С� 

/*

#include <string>
#include <complex>
#include <ctime> 
#include <cstring> //strlen()
#include <type_traits>
#include <memory>
*/

using namespace std;

//----------------------------------------------------
#include <iostream>
#include <complex>
#include <memory>				 //std::allocator  
#include <ext\pool_allocator.h>	 //��ʹ�� std::allocator ����� allocator, �͵����� #include <ext/...> 
namespace jj01
{
void test_primitives()
{
	cout << "\ntest_primitives().......... \n";
	
    void* p1 = malloc(512);	//512 bytes
    free(p1);

    complex<int>* p2 = new complex<int>; //one object
    delete p2;             

    void* p3 = ::operator new(512); //512 bytes
    ::operator delete(p3);

//����ʹ�� C++ �˜ʎ��ṩ�� allocators��
//��ӿ��m�И˜�Ҏ�񣬵����F�S�́Kδ��ȫ���أ�����������ʽ�Ԯ���
#ifdef _MSC_VER
    //���ɺ������� non-static����Ҫͨ�^ object �{�á����·��� 3 �� ints.
    int* p4 = allocator<int>().allocate(3, (int*)0); 
    allocator<int>().deallocate(p4,3);           
#endif
#ifdef __BORLANDC__
    //���ɺ������� non-static����Ҫͨ�^ object �{�á����·��� 5 �� ints.
    int* p4 = allocator<int>().allocate(5);  
    allocator<int>().deallocate(p4,5);       
#endif
#ifdef __GNUC__
    //���ɺ������� static����ͨ�^ȫ���{��֮�����·��� 512 bytes.
    //void* p4 = alloc::allocate(512); 
    //alloc::deallocate(p4,512);   
    
    //���ɺ������� non-static����Ҫͨ�^ object �{�á����·��� 7 �� ints.    
	void* p4 = allocator<int>().allocate(7); 
    allocator<int>().deallocate((int*)p4,7);     
	
    //���ɺ������� non-static����Ҫͨ�^ object �{�á����·��� 9 �� ints.	
	void* p5 = __gnu_cxx::__pool_alloc<int>().allocate(9); 
    __gnu_cxx::__pool_alloc<int>().deallocate((int*)p5,9);	
#endif
}	
} //namespace
//----------------------------------------------------
#include <iostream>
#include <string>
//#include <memory>				 //std::allocator  

namespace jj02
{
	
class A
{
public:
  int id;
  
  A() : id(0)      { cout << "default ctor. this="  << this << " id=" << id << endl;  }
  A(int i) : id(i) { cout << "ctor. this="  << this << " id=" << id << endl;  }
  ~A()             { cout << "dtor. this="  << this << " id=" << id << endl;  }
};
	
void test_call_ctor_directly()
{
	cout << "\ntest_call_ctor_directly().......... \n";	
	
    string* pstr = new string;
    cout << "str= " << *pstr << endl;
    
//! pstr->string::string("jjhou");  
                        //[Error] 'class std::basic_string<char>' has no member named 'string'
//! pstr->~string();	//crash -- ���Z���Z�ⶼ�����_��, crash ֻ�����һ�б� remark ������.  
    cout << "str= " << *pstr << endl;


//------------

  	A* pA = new A(1);         	//ctor. this=000307A8 id=1
  	cout << pA->id << endl;   	//1
//!	pA->A::A(3);                //in VC6 : ctor. this=000307A8 id=3
  								//in GCC : [Error] cannot call constructor 'jj02::A::A' directly
  								
//!	A::A(5);	  				//in VC6 : ctor. this=0013FF60 id=5
                      			//         dtor. this=0013FF60  	
  								//in GCC : [Error] cannot call constructor 'jj02::A::A' directly
  								//         [Note] for a function-style cast, remove the redundant '::A'
		
  	cout << pA->id << endl;   	//in VC6 : 3
  								//in GCC : 1  	
  	
  	delete pA;                	//dtor. this=000307A8 

  	//simulate new
  	void* p = ::operator new(sizeof(A));  
  	cout << "p=" << p << endl; 	//p=000307A8
  	pA = static_cast<A*>(p);
//!	pA->A::A(2);				//in VC6 : ctor. this=000307A8 id=2
  								//in GCC : [Error] cannot call constructor 'jj02::A::A' directly  	
  	
  	cout << pA->id << endl;     //in VC6 : 2
  								//in GCC : 0  	

  	//simulate delete
  	pA->~A();					//dtor. this=000307A8 
  	::operator delete(pA);		//free()
}	
} //namespace
//----------------------------------------------------
#include <iostream>
#include <new>		//placement new
namespace jj03
{
	
using jj02::A;
	
void test_array_new_and_placement_new()
{
	cout << "\ntest_placement_new().......... \n";	
	
size_t size = 3;
	
{
//case 1
//ģ�M memory pool ������, array new + placement new. ���� 
 
   	A* buf = (A*)(new char[sizeof(A)*size]);
   	A* tmp = buf;   
	   
	cout << "buf=" << buf << "  tmp=" << tmp << endl;	
   	
   	for(int i = 0; i < size; ++i)
	    new (tmp++) A(i);  			//3�� ctor 

	cout << "buf=" << buf << "  tmp=" << tmp << endl;
		    
//!	delete [] buf;    	//crash. why?
						//����@�䌍�ǂ� char array������ delete [] buf; ���g������D������ A::~A. 
						// �� array memory layout ���Ҳ����c array Ԫ�؂��� (���� 3) ���P����Ϣ, 
						// -- ������ֶ��e�y (���Ҍ� VC ���J�R����)����Ǳ����� 
	delete buf;     	//dtor just one time, ~[0]	

	cout << "\n\n";
}

{
//case 2
//���^�yԇ�μ��� array new
	
   	A* buf = new A[size];  //default ctor 3 ��. [0]���[1]���[2])
	         //A����� default ctor, ��t [Error] no matching function for call to 'jj02::A::A()'
   	A* tmp = buf;   
	   
	cout << "buf=" << buf << "  tmp=" << tmp << endl;	
   	
   	for(int i = 0; i < size; ++i)
	    new (tmp++) A(i);  		//3�� ctor 

	cout << "buf=" << buf << "  tmp=" << tmp << endl;
		    
	delete [] buf;    //dtor three times (�����淴, [2]���[1]���[0])	
}

{
//case 3	
//���ձ���ԭ��, �ٴ�ģ�M memory pool����, array new + placement new. 	
//��, ������, ��� memory pool ֻ�ǹ��� memory, ���K���� construction, 
//Ҳ���� destruction. ��ֻؓ؟���� memory. 
//���������� void* �� char* ȡ�� memory, ጷ� (�h��)��Ҳ�� void* or char*.  
//������ case 1 ጷ� (�h��) ���� A*. 
//
//���� memory pool ��ʽ�� jj04::test 
}

}	
} //namespace
//----------------------------------------------------
#include <cstddef>
#include <iostream>
namespace jj04
{
//ref. C++Primer 3/e, p.765
//per-class allocator 

class Screen {
public:
    Screen(int x) : i(x) { };
    int get() { return i; }

    void* operator new(size_t);
    void  operator delete(void*, size_t);	//(2)
//! void  operator delete(void*);			//(1) ����һ. ��(1)(2)�K��,���к���ֵĈ��e (�������^�w) 
	    
private:
    Screen* next;
    static Screen* freeStore;
    static const int screenChunk;
private:
    int i;
};
Screen* Screen::freeStore = 0;
const int Screen::screenChunk = 24;

void* Screen::operator new(size_t size)
{
  Screen *p;
  if (!freeStore) {
      //linked list �ǿյģ����Ծ�ȡһ��K memory
      //���º��е��� global operator new
      size_t chunk = screenChunk * size;
      freeStore = p =
         reinterpret_cast<Screen*>(new char[chunk]);
      //������Á��һ��K memory ���� linked list ��С�KС�K��������
      for (; p != &freeStore[screenChunk-1]; ++p)
          p->next = p+1;
      p->next = 0;
  }
  p = freeStore;
  freeStore = freeStore->next;
  return p;
}


//! void Screen::operator delete(void *p)		//(1)
void Screen::operator delete(void *p, size_t)	//(2)����һ 
{
  //�� deleted object �ջز��� free list ǰ��
  (static_cast<Screen*>(p))->next = freeStore;
  freeStore = static_cast<Screen*>(p);
}

//-------------
void test_per_class_allocator_1()
{	
	cout << "\ntest_per_class_allocator_1().......... \n";	
		
   	cout << sizeof(Screen) << endl;		//8	

size_t const N = 100;
Screen* p[N];	

   	for (int i=0; i< N; ++i)
   	     p[i] = new Screen(i);         

	//ݔ��ǰ 10 �� pointers, ���Ա��^���g�� 
   	for (int i=0; i< 10; ++i)  	   
		cout << p[i] << endl;     
    
   	for (int i=0; i< N; ++i)
   	     delete p[i];     	
}
} //namespace
//----------------------------------------------------
#include <cstddef>
#include <iostream>
namespace jj05
{
//ref. Effective C++ 2e, item10
//per-class allocator 

class Airplane {   //֧Ԯ customized memory management
private:
  	struct AirplaneRep {
    	unsigned long miles;
    	char type;
  	};
private:
  	union {
    	AirplaneRep rep;  //��ᘌ� used object
    	Airplane* next;   //��ᘌ� free list
  	};
public:
  	unsigned long getMiles() { return rep.miles; }
  	char getType() { return rep.type; }
  	void set(unsigned long m, char t)
  	{
     	rep.miles = m;
     	rep.type = t;
  	}
public:
  	static void* operator new(size_t size);
  	static void  operator delete(void* deadObject, size_t size);
private:
  	static const int BLOCK_SIZE;
  	static Airplane* headOfFreeList;
};

Airplane* Airplane::headOfFreeList;  
const int Airplane::BLOCK_SIZE = 512;   

void* Airplane::operator new(size_t size)
{
  	//�����С�e�`���D���o ::operator new()
  	if (size != sizeof(Airplane))
    	return ::operator new(size);

  	Airplane* p = headOfFreeList;  

  	//��� p ��Ч���Ͱ�list�^��������һ��Ԫ��
  	if (p)
    	headOfFreeList = p->next;
  	else {
    	//free list �ѿա�����һ�K���ӛ���w��
    	//������ݼ{ BLOCK_SIZE �� Airplanes
    	Airplane* newBlock = static_cast<Airplane*>
       		(::operator new(BLOCK_SIZE * sizeof(Airplane)));
    	//�M��һ���µ� free list����С�^�K����һ�𣬵����^ 
    	//#0 Ԫ�أ����Ҫ�������ؽo�����ߡ�
    	for (int i = 1; i < BLOCK_SIZE-1; ++i)
      		newBlock[i].next = &newBlock[i+1];
    	newBlock[BLOCK_SIZE-1].next = 0; //��null�Y��

    	// �� p �O���^������ headOfFreeList �O��
    	// ��һ���ɱ��\�õ�С�^�K��
    	p = newBlock;
    	headOfFreeList = &newBlock[1];
  	}
  	return p;
}

// operator delete �ӫ@һ�Kӛ���w��
// ������Ĵ�С���_���Ͱ����ӵ� free list ��ǰ��
void Airplane::operator delete(void* deadObject,
                               size_t size)
{
  	if (deadObject == 0) return;          
  	if (size != sizeof(Airplane)) {   
    	::operator delete(deadObject);
    	return;
  	}

  	Airplane *carcass =
    	static_cast<Airplane*>(deadObject);

  	carcass->next = headOfFreeList;
  	headOfFreeList = carcass;
}

//-------------
void test_per_class_allocator_2() 
{	
	cout << "\ntest_per_class_allocator_2().......... \n";		
	
  	cout << sizeof(Airplane) << endl;    //8

size_t const N = 100;
Airplane* p[N];	

   	for (int i=0; i< N; ++i)
   	     p[i] = new Airplane;     
			
    
    //�S�C�yԇ object ������ 
  	p[1]->set(1000,'A'); 	
  	p[5]->set(2000,'B');
  	p[9]->set(500000,'C');
  	cout << p[1] << ' ' << p[1]->getType() << ' ' << p[1]->getMiles() << endl;
  	cout << p[5] << ' ' << p[5]->getType() << ' ' << p[5]->getMiles() << endl;
  	cout << p[9] << ' ' << p[9]->getType() << ' ' << p[9]->getMiles() << endl; 
  	
	//ݔ��ǰ 10 �� pointers, ���Ա��^���g�� 
   	for (int i=0; i< 10; ++i)  	   
		cout << p[i] << endl; 		 
	 
   	for (int i=0; i< N; ++i)
   	     delete p[i]; 	
}
} //namespace
//----------------------------------------------------
#include <cstddef>
#include <iostream>
#include <string>
namespace jj06
{

class Foo
{
public:
  int _id;
  long _data;
  string _str;
  
public:
  	static void* operator new(size_t size);
  	static void  operator delete(void* deadObject, size_t size);
  	static void* operator new[](size_t size);
  	static void  operator delete[](void* deadObject, size_t size);	  	  
  
  Foo() : _id(0)      { cout << "default ctor. this="  << this << " id=" << _id << endl;  }
  Foo(int i) : _id(i) { cout << "ctor. this="  << this << " id=" << _id << endl;  }
  //virtual 
  ~Foo()              { cout << "dtor. this="  << this << " id=" << _id << endl;  }
  
  //���� virtual dtor, sizeof = 12, new Foo[5] => operator new[]() �� size ������ 64, 
  //���� virtual dtor, sizeof = 16, new Foo[5] => operator new[]() �� size ������ 84, 
  //���������������� 4 ���ܾ��ǂ� size_t ��λ�Á���� array size. 
};

void* Foo::operator new(size_t size)
{
  	Foo* p = (Foo*)malloc(size);  
	cout << "Foo::operator new(), size=" << size << "\t  return: " << p << endl;  	

  	return p;
}

void Foo::operator delete(void* pdead, size_t size)
{
	cout << "Foo::operator delete(), pdead= " << pdead << "  size= " << size << endl;
	free(pdead);
}

void* Foo::operator new[](size_t size)
{
  	Foo* p = (Foo*)malloc(size);  //crash, ���}���ܳ����@�� 
	cout << "Foo::operator new[](), size=" << size << "\t  return: " << p << endl;  
	
  	return p;
}

void Foo::operator delete[](void* pdead, size_t size)
{
	cout << "Foo::operator delete[](), pdead= " << pdead << "  size= " << size << endl;
	
	free(pdead);
}
	
//-------------	
void test_overload_operator_new_and_array_new() 
{	
	cout << "\ntest_overload_operator_new_and_array_new().......... \n";		
	
	cout << "sizeof(Foo)= " << sizeof(Foo) << endl;
	
	{	
    Foo* p = new Foo(7);
    delete p;
    
    Foo* pArray = new Foo[5];	//�o���o array elements �� initializer 
    delete [] pArray;	
	}
	
	{	    
	cout << "testing global expression ::new and ::new[] \n";
	// �@���@�^ overloaded new(), delete(), new[](), delete[]() 
	// ����Ȼ ctor, dtor ��������������.  
	
    Foo* p = ::new Foo(7);
    ::delete p;
    
    Foo* pArray = ::new Foo[5];	
    ::delete [] pArray;
	}
}
} //namespace
//----------------------------------------------------
#include <vector>  //for test

namespace jj07
{

class Bad { };
class Foo
{
public:
  	Foo() { cout << "Foo::Foo()" << endl;  }
  	Foo(int) { 
	   			cout << "Foo::Foo(int)" << endl;  
	           	// throw Bad();  
			 }

  	//(1) �@������һ��� operator new() �����d 
  	void* operator new(size_t size) {
		cout << "operator new(size_t size), size= " << size << endl;
    	return malloc(size);  
  	}

  	//(2) �@�����ǘ˜ʎ��ѽ��ṩ�� placement new() �����d (��ʽ)
	//    (������Ҳģ�M standard placement new �Ą���, just return ptr) 
  	void* operator new(size_t size, void* start) { 
	  	cout << "operator new(size_t size, void* start), size= " << size << "  start= " << start << endl;
    	return start;
  	}

  	//(3) �@�����Ǎ��µ� placement new 
  	void* operator new(size_t size, long extra) { 
	  	cout << "operator new(size_t size, long extra)  " << size << ' ' << extra << endl;
    	return malloc(size+extra);
  	}

  	//(4) �@����һ�� placement new 
  	void* operator new(size_t size, long extra, char init) { 
	  	cout << "operator new(size_t size, long extra, char init)  " << size << ' ' << extra << ' ' << init << endl;
    	return malloc(size+extra);
  	}
  	
   	//(5) �@����һ�� placement new, �����⌑�e��һ������ type (������� size_t �ԝM�������� operator new) 
//!  	void* operator new(long extra, char init) { //[Error] 'operator new' takes type 'size_t' ('unsigned int') as first parameter [-fpermissive]
//!	  	cout << "op-new(long,char)" << endl;
//!    	return malloc(extra);
//!  	} 	

    //�����Ǵ������� placement new �ĸ��� called placement delete. 
	//�� ctor �l���������@�������� operator (placement) delete �͕�������. 
	//��ԓ��Ҫؓ؟ጷ����n�ֵ� (placement new) �������õ� memory.  
  	//(1) �@������һ��� operator delete() �����d 
  	void operator delete(void*,size_t)
  	{ cout << "operator delete(void*,size_t)  " << endl;  }

	//(2) �@�ǌ��������� (2)  
  	void operator delete(void*,void*)
  	{ cout << "operator delete(void*,void*)  " << endl;  }

	//(3) �@�ǌ��������� (3)  
  	void operator delete(void*,long)
  	{ cout << "operator delete(void*,long)  " << endl;  }

	//(4) �@�ǌ��������� (4)  
	//����]��һһ����, Ҳ�������κξ��g���e 
  	void operator delete(void*,long,char)
  	{ cout << "operator delete(void*,long,char)  " << endl; }
  	
private:
  	int m_i;
};


//-------------	
void test_overload_placement_new()
{
	cout << "\n\n\ntest_overload_placement_new().......... \n";
	
  	Foo start;  //Foo::Foo

  	Foo* p1 = new Foo;           //op-new(size_t)
  	Foo* p2 = new (&start) Foo;  //op-new(size_t,void*)
  	Foo* p3 = new (100) Foo;     //op-new(size_t,long)
  	Foo* p4 = new (100,'a') Foo; //op-new(size_t,long,char)

  	Foo* p5 = new (100) Foo(1);     //op-new(size_t,long)  op-del(void*,long)
  	Foo* p6 = new (100,'a') Foo(1); //
  	Foo* p7 = new (&start) Foo(1);  //
  	Foo* p8 = new Foo(1);           //
  	//VC6 warning C4291: 'void *__cdecl Foo::operator new(unsigned int)'
  	//no matching operator delete found; memory will not be freed if
  	//initialization throws an exception
}
} //namespace	
//----------------------------------------------------
#include <cstdlib>	//malloc, free

void* myAlloc(size_t size)
{  return malloc(size); }

void myFree(void* ptr)
{  return free(ptr); }

//��Ҫ�OӋһ��������Ӌ���������Ϳ�ጷ����� operator new() / operator delete().
//���� user ֱ��ʹ�� malloc/free, ��t���ܲ��_����, �@�ӾͿ����۷e����. 
static long long countNew = 0;
static long long countDel = 0;	    
static long long countArrayNew = 0;
static long long countArrayDel = 0;
static long long timesNew = 0;

//С�ģ��@Ӱ푟o�h���� 
//�������ɱ����һ�� namespace �� 
//�����ǳɹ���, ���ҕ��r����Ҫ�� (��Ì����@�����yԇ���򎧁�Ӱ�). 
/*
	inline void* operator new(size_t size) 	 
	{
		//cout << "jjhou global new(), \t" << size << "\t"; 
		countNew += size; 
		++timesNew;  
		
		//void* p = myAlloc( size ); 
		//cout << p << endl; 
	    //return p;
	    
	    return myAlloc( size ); 	    
	}
	
	inline void* operator new[](size_t size)  
	{ 
		cout << "jjhou global new[](), \t" << size << "\t";
		countArrayNew += size; 				

		void* p = myAlloc( size ); 
		cout << p << endl; 
	    return p;
	    
		//return myAlloc( size ); 	    
	}	
	
	//�찡, ����(1)(2)���ԁK��K��(2)ץס���� (���������@���Ĝyԇ�o��). 
	//��ֻ���� (1) �r, ץ��ס����. 
	//�� class members �ж���ֻ�ܓ�һ (��һ����) 
	//(1) 
	inline void  operator delete(void* ptr, size_t size)  
	{  	
 		cout << "jjhou global delete(ptr,size), \t" << ptr << "\t" << size << endl;	
		countDel += size;  	
	 	myFree( ptr ); 			
	}
	//(2)
	inline void  operator delete(void* ptr)  
	{  	
 		//cout << "jjhou global delete(ptr), \t" << ptr << endl;	 	
	 	myFree( ptr ); 			
	}	
	
	//(1)
	inline void  operator delete[](void* ptr, size_t size) 
	{ 
	    cout << "jjhou global delete[](ptr,size), \t" << ptr << "\t" << size << endl;
	    countArrayDel += size; 
		myFree( ptr ); 			
	}	
	//(2)
	inline void  operator delete[](void* ptr) 
	{ 
	    cout << "jjhou global delete[](ptr), \t" << ptr << endl;
		myFree( ptr ); 			
	}
*/

#include <vector>
#include <list>
#include <ext\pool_allocator.h> 

//C++/11 alias template
template <typename T>
using listPool = list<T, __gnu_cxx::__pool_alloc<T>>; 	
	
namespace jj08
{
//-------------	
void test_overload_global_new()
{
	cout << "\n\n\ntest_overload_global_new().......... \n";

//***** �yԇ�r, main() �е������yԇȫ�� remark, �������yԇ *****
  { 
    cout << "::countNew= " << ::countNew << endl;		//0
    cout << "::countDel= " << ::countDel << endl;   	//0 
    cout << "::timesNew= " << ::timesNew << endl;   	//0 
    
    string* p = new string("My name is Ace");		//jjhou global new(), 4 	(�]���@�� string size) 
    												//jjhou global new(), 27	(�]���@�� sizeof(Rep)+extra) 
    delete p;		//jjhou global delete(ptr), 0x3e3e48
    				//jjhou global delete(ptr), 0x3e3e38
    
    cout << "::countNew= " << ::countNew << endl;	//31 ==> 4+27
    cout << "::timesNew= " << ::timesNew << endl;   //2
    cout << "::countDel= " << ::countDel << endl;	//0 <== ���yԇ�@Ȼ�����h�^�첻������Ҫ�^���
	               									//      ����M��ȥ operator delete(ptr,size) �� 	               								
	    
	p = new string[3];		//jjhou global new[](), 16 (�]�����ЃȺ� arraySize field: 4 bytes, 
													  	//���� 16-4 = 12 ==> 4*3, Ҳ���� 3 �� string ÿ���� 4 bytes)
							//jjhou global new(), 13  	//Nil string
							//jjhou global new(), 13	//Nil string	
							//jjhou global new(), 13	//Nil string										
							
    delete [] p;			//jjhou global delete(ptr),   0x3e3e88
							//jjhou global delete(ptr),   0x3e3e70
							//jjhou global delete(ptr),   0x3e39c8
							//jjhou global delete[](ptr), 0x3e3978   
							
    cout << "::countNew= " << ::countNew << endl;			//70 ==> 4+27+13+13+13
    cout << "::timesNew= " << ::timesNew << endl;   		//5    
    cout << "::countArrayNew= " << ::countArrayNew << endl;	//16 (�@����ֵ�䌍���Ҷ��ԟo���x)
	
	//�yԇ: global operator new Ҳ������������Ӱ�
	vector<int> vec(10);	//jjhou global new(), 	40  	0x3e3ea0  (�]��10 ints)
							//�]��vector object ������ dynamic allocated. 
	vec.push_back(1); 		
	  		 				//jjhou global new(), 	80		0x3e3ed0
							//jjhou global delete(ptr), 	0x3e3ea0	
	vec.push_back(1); 		
	vec.push_back(1); 	
								
    cout << "::countNew= " << ::countNew << endl;	//190 ==> 70+40+80		
    cout << "::timesNew= " << ::timesNew << endl;   //7    
	
	list<int> lst;		//�]��list object ������ dynamic allocated. 	
	lst.push_back(1); 	//jjhou global new(), 	12	(�]��ÿ�� node�� 12 bytes) 
	lst.push_back(1); 	//jjhou global new(), 	12
	lst.push_back(1); 	//jjhou global new(), 	12
    cout << "::countNew= " << ::countNew << endl;	//226 ==> 190+12+12+12		
    cout << "::timesNew= " << ::timesNew << endl;   //10  
			
	//jjhou global delete(ptr), 	0x3e3978
	//jjhou global delete(ptr), 	0x3e39c8
	//jjhou global delete(ptr), 	0x3e3e70
	//jjhou global delete(ptr), 	0x3e3ed0	
  }
	
  {
  	//reset countNew
	countNew = 0;
	timesNew = 0;	
	
  	//list<double, __gnu_cxx::__pool_alloc<double>> lst;	
	//��һ�и��� C++/11 alias template �팑 : 
	listPool<double> lst; 

  	for (int i=0; i< 1000000; ++i)
  	    lst.push_back(i);
    cout << "::countNew= " << ::countNew << endl;  	//16752832 (ע��, node ������ cookie)
    cout << "::timesNew= " << ::timesNew << endl;   //122	
  }
  
  {
  	//reset countNew
	countNew = 0;
	timesNew = 0;	
  	list<double> lst;
  	for (int i=0; i< 1000000; ++i)
  	    lst.push_back(i);
    cout << "::countNew= " << ::countNew << endl;	//16000000 (ע��, node ���� cookie)
    cout << "::timesNew= " << ::timesNew << endl;   //1000000    
  }  										 
}
} //namespace
//----------------------------------------------------
namespace jj09
{
	
class allocator 
{
private:
  	struct obj {
    	struct obj* next;  //embedded pointer
  	};	
public:
    void* allocate(size_t);
    void  deallocate(void*, size_t);
    void  check();
    
private: 
    obj* freeStore = nullptr;
    const int CHUNK = 5; //Сһ�c�����^�� 
};

void* allocator::allocate(size_t size)
{
  	obj* p;

  	if (!freeStore) {
      	//linked list �ǿյģ����Ծ�ȡһ��K memory
      	size_t chunk = CHUNK * size;
      	freeStore = p = (obj*)malloc(chunk);  
      
      	//cout << "empty. malloc: " << chunk << "  " << p << endl;
     
      	//������Á��һ��K���� linked list ��С�KС�K��������
      	for (int i=0; i < (CHUNK-1); ++i)	{  //�]����Ư��, �������c�o���^.  
           	p->next = (obj*)((char*)p + size);
           	p = p->next;
      	}
      	p->next = nullptr;  //last       
  	}
  	p = freeStore;
  	freeStore = freeStore->next;
   
  	//cout << "p= " << p << "  freeStore= " << freeStore << endl;
  
  	return p;
}
void allocator::deallocate(void* p, size_t)
{
  	//�� deleted object �ջز��� free list ǰ��
  	((obj*)p)->next = freeStore;
  	freeStore = (obj*)p;
}
void allocator::check()
{ 
    obj* p = freeStore;
    int count = 0;
    
    while (p) {
        cout << p << endl;
		p = p->next;
		count++;
	}
    cout << count << endl;
}
//--------------

class Foo {
public: 
	long L;
	string str;
	static allocator myAlloc;
public:
	Foo(long l) : L(l) {  }
	static void* operator new(size_t size)
  	{     return myAlloc.allocate(size);  	}
  	static void  operator delete(void* pdead, size_t size)
    {     return myAlloc.deallocate(pdead, size);  }
};
allocator Foo::myAlloc;


class Goo {
public: 
	complex<double> c;
	string str;
	static allocator myAlloc;
public:
	Goo(const complex<double>& x) : c(x) {  }
	static void* operator new(size_t size)
  	{     return myAlloc.allocate(size);  	}
  	static void  operator delete(void* pdead, size_t size)
    {     return myAlloc.deallocate(pdead, size);  }
};
allocator Goo::myAlloc;

//-------------	
void test_static_allocator_3()
{
	cout << "\n\n\ntest_static_allocator().......... \n";	

{	
Foo* p[100];
 
	cout << "sizeof(Foo)= " << sizeof(Foo) << endl;
   	for (int i=0; i<23; ++i) {	//23,���┵, �S�⿴���Y�� 
	   	p[i] = new Foo(i); 
	    cout << p[i] << ' ' << p[i]->L << endl;
	}
	//Foo::myAlloc.check();
	
   	for (int i=0; i<23; ++i) {
	   	delete p[i]; 
	}	
	//Foo::myAlloc.check();
}

{	
Goo* p[100];
 
	cout << "sizeof(Goo)= " << sizeof(Goo) << endl;
   	for (int i=0; i<17; ++i) {	//17,���┵, �S�⿴���Y�� 
	   	p[i] = new Goo(complex<double>(i,i)); 
	    cout << p[i] << ' ' << p[i]->c << endl;
	}
	//Goo::myAlloc.check();
	
   	for (int i=0; i<17; ++i) {
	   	delete p[i]; 
	}	
	//Goo::myAlloc.check();	
}
}
} //namespace	
//----------------------------------------------------	
namespace jj10
{	
using jj09::allocator;

//���R MFC macros. 
// DECLARE_POOL_ALLOC -- used in class definition
#define DECLARE_POOL_ALLOC() \
public: \
    void* operator new(size_t size) { return myAlloc.allocate(size); } \
    void operator delete(void* p) { myAlloc.deallocate(p, 0); } \
protected: \
    static allocator myAlloc; 

// IMPLEMENT_POOL_ALLOC -- used in class implementation file
#define IMPLEMENT_POOL_ALLOC(class_name) \
allocator class_name::myAlloc; 


// in class definition file
class Foo {
   DECLARE_POOL_ALLOC()
public: 
	long L;
	string str;
public:
	Foo(long l) : L(l) {  }   
};
//in class implementation file
IMPLEMENT_POOL_ALLOC(Foo) 


//  in class definition file
class Goo {
   DECLARE_POOL_ALLOC()
public: 
	complex<double> c;
	string str;
public:
	Goo(const complex<double>& x) : c(x) {  }   
};
//in class implementation file
IMPLEMENT_POOL_ALLOC(Goo) 


void test_macros_for_static_allocator()
{
	cout << "\n\n\ntest_macro_for_static_allocator().......... \n";
		
Foo* pF[100];
Goo* pG[100];
 
	cout << "sizeof(Foo)= " << sizeof(Foo) << endl;
	cout << "sizeof(Goo)= " << sizeof(Goo) << endl;	
	
   	for (int i=0; i<23; ++i) {	//23,���┵, �S�⿴���Y�� 
	   	pF[i] = new Foo(i); 
	   	pG[i] = new Goo(complex<double>(i,i)); 	   	
	    cout << pF[i] << ' ' << pF[i]->L << "\t";
	    cout << pG[i] << ' ' << pG[i]->c << "\n";	    
	}
	
   	for (int i=0; i<23; ++i) {
	   	delete pF[i]; 
	   	delete pG[i]; 	   	
	}	
	
}
} //namespace
//----------------------------------------------------
namespace jj11
{	
//��̎��ȫģ�� SGI STL, G2.92 �� std::alloc  
//���� namespace ����˺� std ���nͻ 
//���ַ��� G4.92 ext\__pool_alloc.h Ҳ��ȫ��ͬ.  

 
#define __THROW_BAD_ALLOC  cerr << "out of memory" << endl; exit(1)
//----------------------------------------------
// ��1����������
//----------------------------------------------
template <int inst>
class __malloc_alloc_template {
private:
  static void* oom_malloc(size_t);
  static void* oom_realloc(void *, size_t);
  static void (*__malloc_alloc_oom_handler)();

public:
  static void* allocate(size_t n)
  {
    void *result = malloc(n);   //ֱ��ʹ�� malloc()
    if (0 == result) result = oom_malloc(n);
    return result;
  }
  static void deallocate(void *p, size_t /* n */)
  {
    free(p);                    //ֱ��ʹ�� free()
  }
  static void* reallocate(void *p, size_t /* old_sz */, size_t new_sz)
  {
    void * result = realloc(p, new_sz); //ֱ��ʹ�� realloc()
    if (0 == result) result = oom_realloc(p, new_sz);
    return result;
  }
  static void (*set_malloc_handler(void (*f)()))()
  { //��� C++ �� set_new_handler().
    void (*old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = f;
    return(old);
  }
};
//----------------------------------------------
template <int inst>
void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
  void (*my_malloc_handler)();
  void* result;

  for (;;) {    //�����Lԇጷš����á���ጷš������á�
    my_malloc_handler = __malloc_alloc_oom_handler;
    if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
    (*my_malloc_handler)();    //����̎��ʽ����Dጷ�ӛ���w
    result = malloc(n);        //�ٴ·Lԇ����ӛ���w
    if (result) return(result);
  }
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
  void (*my_malloc_handler)();
  void* result;

  for (;;) {    //�����Lԇጷš����á���ጷš������á�
    my_malloc_handler = __malloc_alloc_oom_handler;
    if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
    (*my_malloc_handler)();    //����̎��ʽ����Dጷ�ӛ���w��
    result = realloc(p, n);    //�ٴ·Lԇ����ӛ���w��
    if (result) return(result);
  }
}
//----------------------------------------------

typedef __malloc_alloc_template<0>  malloc_alloc;

template<class T, class Alloc>
class simple_alloc {
public:
  static T* allocate(size_t n)
    { return 0 == n? 0 : (T*)Alloc::allocate(n*sizeof(T)); }
  static T* allocate(void)
    { return (T*)Alloc::allocate(sizeof(T)); }
  static void deallocate(T* p, size_t n)
    { if (0 != n) Alloc::deallocate(p, n*sizeof(T)); }
  static void deallocate(T *p)
    { Alloc::deallocate(p, sizeof(T)); }
};
//----------------------------------------------
//�ڶ���������
//----------------------------------------------
enum {__ALIGN = 8};                        //С�^�K�����{߅��
enum {__MAX_BYTES = 128};                  //С�^�K������
enum {__NFREELISTS = __MAX_BYTES/__ALIGN}; //free-lists ����

//�����Ѓɂ� template ������ȫ�]�������È�
template <bool threads, int inst>
class __default_alloc_template {
private:
  //���H�ϑ�ʹ�� static const int x = N
  //ȡ�� enum { x = N }, ��Ŀǰ֧Ԯԓ���|�ľ��g������

  static size_t ROUND_UP(size_t bytes) {
    return (((bytes) + __ALIGN-1) & ~(__ALIGN - 1));
  }

private:
  union obj {
    union obj* free_list_link;
  };

private:
  static obj* volatile free_list[__NFREELISTS];
  static size_t FREELIST_INDEX(size_t bytes) {
    return (((bytes) + __ALIGN-1)/__ALIGN - 1);
  }

  // Returns an object of size n, and optionally adds to size n free list.
  static void *refill(size_t n);

  // Allocates a chunk for nobjs of size "size".  nobjs may be reduced
  // if it is inconvenient to allocate the requested number.
  static char *chunk_alloc(size_t size, int &nobjs);

  // Chunk allocation state.
  static char*  start_free;
  static char*  end_free;
  static size_t heap_size;

public:

  static void * allocate(size_t n)  //n must be > 0
  {
    obj* volatile *my_free_list;    //obj** my_free_list;
    obj* result;

    if (n > (size_t)__MAX_BYTES) {
        return(malloc_alloc::allocate(n));
    }

    my_free_list = free_list + FREELIST_INDEX(n);
    result = *my_free_list;
    if (result == 0) {
        void* r = refill(ROUND_UP(n));
        return r;
    }

    *my_free_list = result->free_list_link;
    return (result);
  }

  static void deallocate(void *p, size_t n)  //p may not be 0
  {
    obj* q = (obj*)p;
    obj* volatile *my_free_list;   //obj** my_free_list;

    if (n > (size_t) __MAX_BYTES) {
        malloc_alloc::deallocate(p, n);
        return;
    }
    my_free_list = free_list + FREELIST_INDEX(n);
    q->free_list_link = *my_free_list;
    *my_free_list = q;
  }

  static void * reallocate(void *p, size_t old_sz, size_t new_sz);

};
//----------------------------------------------
// We allocate memory in large chunks in order to
// avoid fragmentingthe malloc heap too much.
// We assume that size is properly aligned.
// We hold the allocation lock.
//----------------------------------------------
template <bool threads, int inst>
char*
__default_alloc_template<threads, inst>::
chunk_alloc(size_t size, int& nobjs)
{
  char* result;
  size_t total_bytes = size * nobjs;
  size_t bytes_left = end_free - start_free;

  if (bytes_left >= total_bytes) {
      result = start_free;
      start_free += total_bytes;
      return(result);
  } else if (bytes_left >= size) {
      nobjs = bytes_left / size;
      total_bytes = size * nobjs;
      result = start_free;
      start_free += total_bytes;
      return(result);
  } else {
      size_t bytes_to_get =
                 2 * total_bytes + ROUND_UP(heap_size >> 4);
      // Try to make use of the left-over piece.
      if (bytes_left > 0) {
          obj* volatile *my_free_list =
                 free_list + FREELIST_INDEX(bytes_left);

          ((obj*)start_free)->free_list_link = *my_free_list;
          *my_free_list = (obj*)start_free;
      }
      start_free = (char*)malloc(bytes_to_get);
      if (0 == start_free) {
          int i;
          obj* volatile *my_free_list, *p;

          //Try to make do with what we have. That can't
          //hurt. We do not try smaller requests, since that tends
          //to result in disaster on multi-process machines.
          for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
              my_free_list = free_list + FREELIST_INDEX(i);
              p = *my_free_list;
              if (0 != p) {
                  *my_free_list = p -> free_list_link;
                  start_free = (char*)p;
                  end_free = start_free + i;
                  return(chunk_alloc(size, nobjs));
                  //Any leftover piece will eventually make it to the
                  //right free list.
              }
          }
          end_free = 0;       //In case of exception.
          start_free = (char*)malloc_alloc::allocate(bytes_to_get);
          //This should either throw an exception or
          //remedy the situation. Thus we assume it
          //succeeded.
      }
      heap_size += bytes_to_get;
      end_free = start_free + bytes_to_get;
      return(chunk_alloc(size, nobjs));
  }
}
//----------------------------------------------
// Returns an object of size n, and optionally adds
// to size n free list.We assume that n is properly aligned.
// We hold the allocation lock.
//----------------------------------------------
template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::
refill(size_t n)
{
    int nobjs = 20;
    char* chunk = chunk_alloc(n,nobjs);
    obj* volatile *my_free_list;   //obj** my_free_list;
    obj* result;
    obj* current_obj;
    obj* next_obj;
    int i;

    if (1 == nobjs) return(chunk);
    my_free_list = free_list + FREELIST_INDEX(n);

    //Build free list in chunk
    result = (obj*)chunk;
    *my_free_list = next_obj = (obj*)(chunk + n);
    for (i=1;  ; ++i) {
      current_obj = next_obj;
      next_obj = (obj*)((char*)next_obj + n);
      if (nobjs-1 == i) {
          current_obj->free_list_link = 0;
          break;
      } else {
          current_obj->free_list_link = next_obj;
      }
    }
    return(result);
}
//----------------------------------------------
template <bool threads, int inst>
char *__default_alloc_template<threads,inst>::start_free = 0;

template <bool threads, int inst>
char *__default_alloc_template<threads,inst>::end_free = 0;

template <bool threads, int inst>
size_t __default_alloc_template<threads,inst>::heap_size = 0;

template <bool threads, int inst>
typename __default_alloc_template<threads, inst>::obj* volatile
__default_alloc_template<threads, inst>::free_list[__NFREELISTS]
     = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
//----------------------------------------------
//���2�������������Q�� alloc
typedef __default_alloc_template<false,0> alloc;


//**********************************

void test_G29_alloc()
{
	cout << "\n\n\ntest_global_allocator_with_16_freelist().......... \n";
	
void*	p1 = alloc::allocate(120);
void* 	p2 = alloc::allocate(72);	
void* 	p3 = alloc::allocate(60);	//���� 8 ���� 

    cout << p1 << ' ' << p2 << ' ' << p3 << endl;

	alloc::deallocate(p1,120);
	alloc::deallocate(p2,72);
	alloc::deallocate(p3,60);	
	
//����, ���ܴ���������yԇ, ����°� G++ ��� allocator �и���Ҫ�� (ԃ������ typedef �� alloc ���o���ش�) 
//���䌍���� G4.9 __pool_alloc������׌ G4.9����ʹ�� __pool_alloc Ҳ�͵�ͬ��@�e��Ҫ�Ĝyԇ 
/*	
	vector<int, simple_alloc<int,alloc>> v;
	for(int i=0; i< 1000; ++i)
		v.push_back(i);
	for(int i=700; i< 720; ++i)
		cout << v[i] << ' ';
*/
			
	cout << endl;	
}
} //namespace
//----------------------------------------------------
namespace jj12
{
	
class Foo {
public: 
	long _x;
public:
	Foo(long x=0) : _x(x) {  }
	
//!	static void* operator new(size_t size) = default;	 				//[Error] cannot be defaulted
//! static void  operator delete(void* pdead, size_t size) = default;	//[Error] cannot be defaulted
	static void* operator new[](size_t size) = delete;
  	static void  operator delete[](void* pdead, size_t size) = delete;
};

class Goo {
public: 
	long _x;
public:
	Goo(long x=0) : _x(x) {  }
	
	static void* operator new(size_t size) = delete;
  	static void  operator delete(void* pdead, size_t size) = delete;
};
	
void test_delete_and_default_for_new()
{	
	cout << "\n\n\ntest_delete_and_default_for_new().......... \n";

    Foo* p1 = new Foo(5);
    delete p1;
//! Foo* pF = new Foo[10];	//[Error] use of deleted function 'static void* jj12::Foo::operator new [](size_t)'
//!	delete [] pF;			//[Error] use of deleted function 'static void jj12::Foo::operator delete [](void*, size_t)'
	 
//! Goo* p2 = new Goo(7);	//[Error] use of deleted function 'static void* jj12::Goo::operator new(size_t)'
//!	delete p2;				//[Error] use of deleted function 'static void jj12::Goo::operator delete(void*, size_t)'
    Goo* pG = new Goo[10];
	delete [] pG;
}
} //namespace
//----------------------------------------------------
#include <new>
#include <iostream>
#include <cassert>
namespace jj13
{
void noMoreMemory()
{
    cerr << "out of memory";
    abort();  
}
	
void test_set_new_handler()
{	
	cout << "\n\n\ntest_set_new_handler().......... \n";

    set_new_handler(noMoreMemory);

/*
    int* p = new int[100000000000000];   //well, so BIG!
    assert(p);

    p = new int[100000000000000000000];  //[Warning] integer constant is too large for its type
    assert(p);
*/

}
} //namespace
//----------------------------------------------------
namespace jj14
{
#include "allocc.h"
#include "stdio.h"   //printf()
#include "stdlib.h"	 //malloc(),free()

//----------------------------------------------
void test_G29_alloc_C_version()
{
	cout << "\n\n\ntest_G29_alloc_C_version.......... \n";

  //����ʹ�� printf() ģ�M C �h����. 
  {
  void* p1 = malloc(16);
  void* p2 = malloc(16);
  void* p3 = malloc(16);
  void* p4 = malloc(16);

  printf("%p \n", p1);
  printf("%p \n", p2);
  printf("%p \n", p3);
  printf("%p \n", p4);

  free(p1);
  free(p2);
  free(p3);
  free(p4);
  }

  {
  void* p1 = allocate(16);
  void* p2 = allocate(16);
  void* p3 = allocate(16);
  void* p4 = allocate(16);

  printf("%p \n", p1);
  printf("%p \n", p2);
  printf("%p \n", p3);
  printf("%p \n", p4);

  deallocate(p1,16);
  deallocate(p2,16);
  deallocate(p3,16);
  deallocate(p4,16);
  }

  {
  void* p1 = malloc_allocate(100);
  void* p2 = malloc_allocate(128);
  void* p3 = malloc_allocate(128);
  void* p4 = malloc_allocate(128);

  printf("%p \n", p1);
  printf("%p \n", p2);
  printf("%p \n", p3);
  printf("%p \n", p4);

  malloc_deallocate(p1,100);
  malloc_deallocate(p2,128);
  malloc_deallocate(p3,128);
  malloc_deallocate(p4,128);
  }

  {
  void* p1 = allocate(100);
  void* p2 = allocate(128);
  void* p3 = allocate(128);
  void* p4 = allocate(128);

  printf("%p \n", p1);
  printf("%p \n", p2);
  printf("%p \n", p3);
  printf("%p \n", p4);

  deallocate(p1,100);
  deallocate(p2,128);
  deallocate(p3,128);
  deallocate(p4,128);
  }

}
} //namespace
//----------------------------------------------------


#ifdef __GNUC__  

#include <cstddef>
#include <memory>				 //�Ⱥ� std::allocator  
#include <ext\pool_allocator.h>	 //��ʹ�� std::allocator ����� allocator, �͵����� #include <ext/...> 
#include <ext\array_allocator.h>
#include <ext\mt_allocator.h>
#include <ext\debug_allocator.h>
#include <ext\bitmap_allocator.h>
#include <ext\malloc_allocator.h>
#include <ext\throw_allocator.h>
#include <ext\new_allocator.h>  //�@�䌍�ѱ� <memory> included, ������ std:allocator �� base class 
#include <iostream>
#include <list>
#include <deque>
#include <vector>
namespace jj33
{
	
//�������� http://www.cplusplus.com/reference/memory/allocator_traits/ 
template <class T>
struct custom_allocator {
  	typedef T value_type;
  	custom_allocator() noexcept {}
  	template <class U> custom_allocator (const custom_allocator<U>&) noexcept {}
//! T* allocate (size_t n) { return static_cast<T*>(::new(n*sizeof(T))); }  
//     	���g���e��new ���Z���� new Type; ���� new N; ������. josuttis 2/e Ҳ����.  
  	T* allocate (size_t n) { return static_cast<T*>(::operator new(n*sizeof(T))); }  
//	void deallocate (T* p, size_t n) { ::delete(p); }	 
//		���g�]���e�������J�鑪ʹ�� operator delete. ������. josuttis 2/e Ҳ����.
  	void deallocate (T* p, size_t n) { ::operator delete(p); }
};

//���µ� operator== �� operator!= ԭ����,����̎Ҳ�S���Ǻ���,��ʹ mark ����Ҳ ok.(Ŀǰ)  
//josuttis 2/e �е� custom allocator Ҳ�Ќ����¶��� operators  
template <class T, class U>
constexpr bool operator== (const custom_allocator<T>&, const custom_allocator<U>&) noexcept
{return true;}

template <class T, class U>
constexpr bool operator!= (const custom_allocator<T>&, const custom_allocator<U>&) noexcept
{return false;}	


/*
template<typename Alloc> 
void cookie_test(Alloc&& alloc, size_t n)	//��춺��Еr�� temp obj (Rvalue) ���룬�����@��ʹ�� &&. ֻ���S��֮�µĴ���
                                            //ʹ�� &&�����N���Еr�Ͳ����� Lvalue ����.  
*/                                            
//����, pass by Rvalue reference �� OK ��. 
//���Ҳ������N��������, �͸��� pass by value �� 
template<typename Alloc> 
void cookie_test(Alloc alloc, size_t n)                                                                                
{
    typename Alloc::value_type *p1, *p2, *p3;		//���� typename 
  	p1 = alloc.allocate(n); 		//allocate() and deallocate() �� non-static, ���� object ����֮. 
  	p2 = alloc.allocate(n);   	
  	p3 = alloc.allocate(n);  

  	cout << "p1= " << p1 << '\t' << "p2= " << p2 << '\t' << "p3= " << p3 << '\n';
	  	
  	alloc.deallocate(p1,sizeof(typename Alloc::value_type)); 	//���� typename 
  	alloc.deallocate(p2,sizeof(typename Alloc::value_type));  	//��Щ allocator ��� 2nd argument ��ֵ�o���^  	
  	alloc.deallocate(p3,sizeof(typename Alloc::value_type)); 	
}

void test_GNU_allocators()
{
  	cout << "\ntest_GNU_allocators().......\n";    	
	
  	//�������� http://www.cplusplus.com/reference/memory/allocator_traits/ 
  	vector<int,custom_allocator<int>> foo {10,20,30};
  	for (auto x : foo) cout << x << " ";		//10, 20, 30
  	cout << '\n';
	  
	  
	cout << sizeof(std::allocator<int>) << endl;			//1
    cout << sizeof(__gnu_cxx::new_allocator<int>) << endl;	//1. 
		//�^�� STL source ��֪: new_allocator �� std::allocator �� base 
		//�҂��o����׃ std::allocator �� base class, ��ԓ���ʹ�������� GNU allocators ? 
		//�Ƿ�Ҫ���� custom_allocator (������) �K������������Ҫ�� base (���� __pool_alloc) ?
		//�������أ���ֱ��ʹ��, �������� #include <ext/...> 
		 
    				
	//���Z����ԇ�ø�ʽ���ӵ� allocators										 
 	cout << sizeof(__gnu_cxx::malloc_allocator<int>) << endl;	//1. ��С 1���䌍�� 0, fields ���� static. 
 	cout << sizeof(__gnu_cxx::__pool_alloc<int>) << endl;		//1
 	cout << sizeof(__gnu_cxx::__mt_alloc<int>) << endl;			//1
 	cout << sizeof(__gnu_cxx::bitmap_allocator<int>) << endl;	//1
 	cout << sizeof(__gnu_cxx::array_allocator<int>) << endl;	//8 ==> �������һ�� ptr ָ�� array ��һ�� size_t ��ʾ���ĵ� array �ă� 
 	cout << sizeof(__gnu_cxx::debug_allocator<std::allocator<double>>) << endl;	//8
//!	cout << sizeof(__gnu_cxx::throw_allocator<int>) << endl;	//ֻ�� throw_allocator_base, throw_allocator_random, throw_allocator_limit, �]�� throw_allocator !! 
		
 	//�������� 
 	list <int, __gnu_cxx::malloc_allocator<int>> list_malloc;  
 	deque <int, __gnu_cxx::debug_allocator<std::allocator<int>>> deque_debug;	
	vector<int, __gnu_cxx::__pool_alloc<int>> vector_pool;
//! vector<int, __pool_alloc<int>> vector_pool;   //����]���� namespace : [Error] '__pool_alloc' was not declared in this scope 
   

    //�yԇ cookie
	cout << "sizeof(int)=" << sizeof(int) << endl;			//4
	cout << "sizeof(double)=" << sizeof(double) << endl;	//8
	 
    cookie_test(std::allocator<int>(), 1);					//��� 10h (��ʾ�� cookie)
    cookie_test(__gnu_cxx::malloc_allocator<int>(), 1);    	//��� 10h (��ʾ�� cookie) 
    cookie_test(__gnu_cxx::__pool_alloc<int>(), 1);			//��� 08h (��ʾ���� cookie) 
    
    //���� int �Ğ� double �Y����׃����ζ���� ints �g�� 8 (���� 4) ������� alignment. 
    cookie_test(std::allocator<double>(), 1);				//��� 10h (��ʾ�� cookie)
    cookie_test(__gnu_cxx::malloc_allocator<double>(), 1);  //��� 10h (��ʾ�� cookie) 
    cookie_test(__gnu_cxx::__pool_alloc<double>(), 1);		//��� 08h (��ʾ���� cookie)     
    
    cout << "sizeof(array<int,3>) = " << sizeof(array<int,3>) << endl;	//12 ==> sizeof(_Alloc_block)=16=10h
    cookie_test(__gnu_cxx::bitmap_allocator<array<int,3>>(), 1);		//��� 10h (��ʾ���� cookie)  
//!    cookie_test(__gnu_cxx::bitmap_allocator<double>(), 1);				//��� 08h (���� cookie) 

     
    list<int, __gnu_cxx::__pool_alloc<int>> list_pool;
    for (int i=0; i<1000; ++i)
        list_pool.push_back(i);
        
   
    try {
		//ʾ��ʹ�� array_allocator: ��� new an array, ���� ptr �O�o array_allocator, ����߀Ҫ delete array  
        std::tr1::array<double,100>* arrayTR1 = new std::tr1::array<double,100>;    //ʹ�� tr1::array 
        cookie_test(__gnu_cxx::array_allocator<double, std::tr1::array<double,100>>(arrayTR1), 1);	//��� 08h (��ʾ���� cookie)
        delete arrayTR1;	


    	array<double,100>* arraySTD = new array<double,100>;	//ʹ�� std::array 
        cookie_test(__gnu_cxx::array_allocator<double, array<double,100>>(arraySTD), 1);  //��� 08h (��ʾ���� cookie) 	
        delete arraySTD;	 
				  				
    	std::tr1::array<double,1>* p = new std::tr1::array<double,1>;	//�������һ�� "default 2nd argument �� std::tr1::array<T,1>" (Ҋ source), �҂���Ҫ��һ����.		
        cookie_test(__gnu_cxx::array_allocator<double>(p), 1);  //δָ�� 2nd argument, ����ʹ�� default, �� std::tr1::array<T,1>
		                                                        //bad allocation! ��� cookie_test() �� 3 doubles �� 
																// ��̎����֮ array_allocator �sֻ���ṩ 1 double�� 
        delete p;	
    }
    catch(...)
    { 
        cout << "bad allocation! \n";
	}
}
}
//----------------------------------------------------

#include <ext\bitmap_allocator.h>	//��ʹ�� std::allocator ����� allocator, �͵����� #include <ext/...> 

namespace jj331
{
	
void test_bitmap_allocators()
{
  	cout << "\ntest_bitmap_allocators().......\n";    	

    __gnu_cxx::bitmap_allocator<double> alloc;   //�γ�һ�� static __mini_vector<pair>, size=0 
    __gnu_cxx::bitmap_allocator<double>::value_type* p[1000];
    __gnu_cxx::bitmap_allocator<double>::value_type* ptr;
	    
	    
	for (int i=0; i<448; ++i) {		//448=64+128+256
	     p[i] = alloc.allocate(1);
		 cout << "p[" << i << "]" << p[i] << endl;
    }
	      
/*	      
	//�����ǂ�ԇ�����S�����N�OӋ...     

	alloc.deallocate(p[31],1); 
	alloc.deallocate(p[32],1); 		
	ptr = alloc.allocate(1);	
	cout << "ptr= " << ptr << endl;
				 				 	
	for (int i=194; i<448; ++i) {  //448=64+128+256
	     p[i] = alloc.allocate(1);
		 cout << "p[" << i << "]" << p[i] << endl;
    }					 	
	
	ptr = alloc.allocate(1);	
	cout << "ptr= " << ptr << endl;
						 	 	
	for (int i=0; i<448; ++i)
	     alloc.deallocate(p[i],1); 
	
	ptr = alloc.allocate(1);	
	cout << "ptr= " << ptr << endl;	
*/	
}
}


#endif // __GNUC__ 

//---------------------------------------------------
int main(int argc, char** argv) 
{
	cout << __cplusplus << endl;

	jj01::test_primitives();	
 
	jj02::test_call_ctor_directly();

	jj03::test_array_new_and_placement_new();	 
 
	jj04::test_per_class_allocator_1();
	
	jj05::test_per_class_allocator_2();	
	
	jj06::test_overload_operator_new_and_array_new();	
	
	jj07::test_overload_placement_new();		
		
	jj08::test_overload_global_new();	
	
	jj09::test_static_allocator_3();
	
	jj10::test_macros_for_static_allocator();			
	
	jj11::test_G29_alloc();
	
	jj12::test_delete_and_default_for_new();	
	
	jj13::test_set_new_handler();	

	jj14::test_G29_alloc_C_version();	
	 
#ifdef __GNUC__
	 
//	jj33::test_GNU_allocators();  //�����ȿ��⒁������,����ƽ�r���yԇ��.  
	jj331::test_bitmap_allocators();
		
#endif	
		
	return 0;
}



