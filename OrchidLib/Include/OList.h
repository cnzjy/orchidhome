/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ��б���ͷ�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OList.h,v 1.4 2006/04/27 02:35:25 zjy Exp $
* ��    ע����ֲ�� Delphi7 �е� TList �б���
*******************************************************************************/

#pragma once

/************************************************************************/
/* �б���                                                               */
/************************************************************************/

typedef int (*TListSortCompare) (void* Item1, void* Item2);

// List �����ڱ���ָ�������б�ʹ�����Է�ʽ���ʡ�
class CList
{
public:
  enum TListNotification {lnAdded, lnExtracted, lnDeleted};

  CList();
  virtual ~CList();

  // �����������������
  int Add(void* Item);
  // ��������
  void Insert(int Index, void* Item);
  // ���������λ��
  void Exchange(int Index1, int Index2);
  // ɾ��һ��
  void Delete(int Index);
  // �Ƴ�һ��
  int Remove(void* Item);
  // ��ֲһ���λ��
  void Move(int CurIndex, int NewIndex);
  // ����б�
  virtual void Clear();
  // ����б��е� NULL ��
  void Pack();
  // ���б���ȡ��һ��Ƴ�
  void* Extract(void* Item);

  // ���������ţ������ڷ��� -1
  int IndexOf(void* Item);

  // ���ش洢��С
  int GetCapacity();
  // ���ô洢��С
  void SetCapacity(int NewCapacity);

  // ���ص�ǰ��Ŀ��
  int GetCount();
  // ���õ�ǰ��Ŀ��
  void SetCount(int NewCount);

  // ��������
  void* First();
  // ����β��
  void* Last();

  // ���б�������
  void Sort(TListSortCompare Compare);

  // ����ָ����
  void* Get(int Index);
  // ����ָ����
  void Put(int Index, void* Item);

protected:
  virtual void Grow();
  virtual void Notify(void* Ptr, TListNotification Action);
  void Error(const char* Msg, int Data);

private:
  void** FList;
  int FCount;
  int FCapacity;

};
