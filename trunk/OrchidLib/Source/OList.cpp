/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：列表类
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OList.cpp,v 1.5 2006/05/01 03:11:13 zjy Exp $
* 备    注：移植自 Delphi7 中的 TList 列表类
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "OList.h"

#define MaxListSize 0x07FFFFFF

const char* SListIndexError = "List index out of bounds (%d)";
const char* SListCapacityError = "List capacity out of bounds (%d)";
const char* SListCountError = "List count out of bounds (%d)";

/************************************************************************/
/* 列表类                                                               */
/************************************************************************/

CList::CList()
{
  FList = NULL;
  FCount = 0;
  FCapacity = 0;
}

CList::~CList()
{
  Clear();
}

int CList::Add(void* Item)
{
  int Result;
  Result = FCount;
  if (Result == FCapacity)
    Grow();
  FList[Result] = Item;
  FCount++;
  if (Item != NULL)
    Notify(Item, lnAdded);
  return Result;
}

void CList::Insert(int Index, void* Item)
{
  if ((Index < 0) || (Index > FCount))
    Error(SListIndexError, Index);
  if (FCount == FCapacity)
    Grow();
  if (Index < FCount)
    for (int i = FCount; i > Index; i--)
      FList[i] = FList[i - 1];
  FList[Index] = Item;
  FCount++;
  if (Item != NULL)
    Notify(Item, lnAdded);
}

void CList::Exchange(int Index1, int Index2)
{
  void* Item;
  if ((Index1 < 0) || (Index1 >= FCount))
    Error(SListIndexError, Index1);
  if ((Index2 < 0) || (Index2 >= FCount))
    Error(SListIndexError, Index2);
  Item = FList[Index1];
  FList[Index1] = FList[Index2];
  FList[Index2] = Item;
}

void CList::Delete(int Index)
{
  void* Temp;
  if ((Index < 0) || (Index >= FCount))
    Error(SListIndexError, Index);
  Temp = FList[Index];
  FCount--;
  if (Index < FCount)
    for (int i = Index; i < FCount; i++)
      FList[i] = FList[i + 1];
  if (Temp != NULL)
    Notify(Temp, lnDeleted);
}

int CList::Remove(void* Item)
{
  int Result;
  Result = IndexOf(Item);
  if (Result >= 0)
    Delete(Result);
  return Result;
}

void CList::Move(int CurIndex, int NewIndex)
{
  void* Item;
  if (CurIndex != NewIndex)
  {
    if ((NewIndex < 0) || (NewIndex >= FCount))
      Error(SListIndexError, NewIndex);
    Item = FList[CurIndex];
    FList[CurIndex] = NULL;
    Delete(CurIndex);
    Insert(NewIndex, NULL);
    FList[NewIndex] = Item;
  }
}

void CList::Clear()
{
  SetCount(0);
  SetCapacity(0);
}

void CList::Pack()
{
  for(int i = FCount - 1; i; i--)
    if (FList[i] = NULL)
      Delete(i);
}

void* CList::Extract(void* Item)
{
  int i;
  void* Result = NULL;
  i = IndexOf(Item);
  if (i >= 0)
  {
    Result = Item;
    FList[i] = NULL;
    Delete(i);
    Notify(Result, lnExtracted);
  }
  return Result;
}

int CList::IndexOf(void* Item)
{
  int Result = 0;
  while (Result < FCount && FList[Result] != Item)
    Result++;
  if (Result == FCount)
    Result = -1;
  return Result;
}

int CList::GetCapacity()
{
  return FCapacity;
}

void CList::SetCapacity(int NewCapacity)
{
  if ((NewCapacity < FCount) || (NewCapacity > MaxListSize))
    Error(SListCapacityError, NewCapacity);
  if (NewCapacity != FCapacity)
  {
    FList = (void**)realloc(FList, NewCapacity * sizeof(void*));
    FCapacity = NewCapacity;
  }
}

int CList::GetCount()
{
  return FCount;
}

void CList::SetCount(int NewCount)
{
  if ((NewCount < 0) || (NewCount > MaxListSize))
    Error(SListCountError, NewCount);
  if (NewCount > FCapacity)
    SetCapacity(NewCount);
  if (NewCount > FCount)
  {
    for (int i = FCount; i < NewCount; i++)
      FList[i] = NULL;
  }
  else
  {
    for (int i = FCount - 1; i >= NewCount; i--)
      Delete(i);
  }
  FCount = NewCount;
}

void* CList::First()
{
  return Get(0);
}

void* CList::Last()
{
  return Get(FCount - 1);
}

void QuickSort(void** SortList, int L, int R, TListSortCompare SCompare)
{
  int i, j;
  void *P, *T;
  do 
  {
    i = L;
    j = R;
    P = SortList[(L + R) >> 1];
    do 
    {
      while (SCompare(SortList[i], P) < 0)
        i++;
      while (SCompare(SortList[j], P) > 0)
        j--;
      if (i <= j)
      {
        T = SortList[i];
        SortList[i] = SortList[j];
        SortList[j] = T;
        i++;
        j--;
      }
    } while(i <= j);

    if (L < j)
      QuickSort(SortList, L, j, SCompare);
    L = i;
  } while(i < R);
}

void CList::Sort(TListSortCompare Compare)
{
  if ((FList != NULL) && (FCount > 0))
    QuickSort(FList, 0, FCount - 1, Compare);
}

void* CList::Get(int Index)
{
  if ((Index < 0) || (Index >= FCount))
    Error(SListIndexError, Index);
  return FList[Index];
}

void CList::Put(int Index, void* Item)
{
  void* Temp;
  if ((Index < 0) || (Index >= FCount))
    Error(SListIndexError, Index);
  if (Item != FList[Index])
  {
    Temp = FList[Index];
    FList[Index] = Item;
    if (Temp != NULL)
      Notify(Temp, lnDeleted);
    if (Item != NULL)
      Notify(Item, lnAdded);
  }
}

void CList::Notify(void* Ptr, TListNotification Action)
{

}

void CList::Grow()
{
  int Delta;
  if (FCapacity > 64)
    Delta = FCapacity / 4;
  else if (FCapacity > 8)
    Delta = 16;
  else
    Delta = 4;
  SetCapacity(FCapacity + Delta);
}

void CList::Error(const char* Msg, int Data)
{
  char buff[255];
  sprintf(buff, Msg, Data);
  throw buff;
}
