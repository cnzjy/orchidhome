/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：列表类头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OList.h,v 1.4 2006/04/27 02:35:25 zjy Exp $
* 备    注：移植自 Delphi7 中的 TList 列表类
*******************************************************************************/

#pragma once

/************************************************************************/
/* 列表类                                                               */
/************************************************************************/

typedef int (*TListSortCompare) (void* Item1, void* Item2);

// List 类用于保存指针或对象列表，使用线性方式访问。
class CList
{
public:
  enum TListNotification {lnAdded, lnExtracted, lnDeleted};

  CList();
  virtual ~CList();

  // 增加新项，返回索引号
  int Add(void* Item);
  // 插入新项
  void Insert(int Index, void* Item);
  // 交换两项的位置
  void Exchange(int Index1, int Index2);
  // 删除一项
  void Delete(int Index);
  // 移除一下
  int Remove(void* Item);
  // 移植一项到新位置
  void Move(int CurIndex, int NewIndex);
  // 清空列表
  virtual void Clear();
  // 清除列表中的 NULL 项
  void Pack();
  // 从列表中取出一项并移除
  void* Extract(void* Item);

  // 返回索引号，不存在返回 -1
  int IndexOf(void* Item);

  // 返回存储大小
  int GetCapacity();
  // 设置存储大小
  void SetCapacity(int NewCapacity);

  // 返回当前项目数
  int GetCount();
  // 设置当前项目数
  void SetCount(int NewCount);

  // 返回首项
  void* First();
  // 返回尾项
  void* Last();

  // 对列表项排序
  void Sort(TListSortCompare Compare);

  // 返回指定项
  void* Get(int Index);
  // 设置指定项
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
