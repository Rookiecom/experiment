#include<stdio.h>
    int EAX;
char a[1000];
void func4(int ecx,int ebx,int esi)
//出去时EAX要为6
/*ecx<7
func4(ecx,0,6)
ecx>3
func4(ecx,4,6)
ecx>5
func4(ecx,6,6)
ecx=6
*/
{   
    EAX=esi;
    EAX-=ebx;
    EAX/=2;
    int edx=EAX+ebx;
    if(edx<=ecx)
    {
        EAX=0;
        if(edx>=ecx)//edx=ecx
        {
            return ;
        }
        else//edx<ecx
        {
            edx++;
            func4(ecx,edx,esi);
            EAX=2*EAX+1;
            return;
        }
    }
    else
    {
        edx--;
        func4(ecx,ebx,edx);
        EAX*=2;
        return ;
    }
}

bool func5()
/*
0x804a060 <array.3249>: 0x00000002      0x0000000a      0x00000006      0x00000001
0x804a070 <array.3249+16>:      0x0000000c      0x00000010      0x00000009      0x00000003
0x804a080 <array.3249+32>:      0x00000004      0x00000007      0x0000000e      0x00000005
0x804a090 <array.3249+48>:      0x0000000b      0x00000008      0x0000000f      0x0000000d
0x31=49=7 * 5 + 14=0x7 * 5 + 0x e
                    9           10
                    ‘9’         ‘：’
*/
{
    int eax,ebx,ecx,edx;        
    ebx=eax;
    ebx+=6;        
    ecx=0;
    while(1)
    {
        edx=(unsigned int)a[eax];
        edx=edx & (0xf);
        //add    0x804a060(,%edx,4),%ecx
        eax++;
        if(eax==ebx) break;
    }
    if(ecx!=31) return false;
}
typedef struct tree
{
    int val;
    tree * left;
    tree * right;
};
tree *head=(tree *)0x804c088;
int Eax;
void func7(tree * edx,int ecx)
{
    int ebx=edx->val;
    if(ebx<=ecx)
    {
        if(ebx==ecx) return ;

    }
}
int main(){
    func4(6,0,14);
    return 0;
}