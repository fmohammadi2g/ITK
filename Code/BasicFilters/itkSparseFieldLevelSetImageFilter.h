/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkSparseFieldLevelSetImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSparseFieldLevelSetImageFilter_h_
#define __itkSparseFieldLevelSetImageFilter_h_

#include "itkFiniteDifferenceImageFilter.h"
#include "itkMultiThreader.h"
#include "itkSparseLevelSetNode.h"

namespace itk {

/**
 * \class SparseFieldLevelSetImageFilter
 *
 *  This layer of the level set solver hierarchy implements
 *  the iterative algorithm for "sparse" iteration, ie. iteration
 *  over active pixels (and related pixels)  in the input and output at each 
 *  change calculation and update step.  This is in contrast to a "dense" 
 *  iteration over all the pixels.
 *
 *  The virtual methods CalculateChange() and ApplyUpdate() specific to sparse
 *  iterations are defined in this object.  This class also implements
 *  threading of the calculations and the updates.
 *  
 *  This filter is applicable to 2D/3D image data.
 *
 * \ingroup ImageFilters
 *
 */
template <class TInputImage, class TOutputImage>
class SparseFieldLevelSetImageFilter  
  : public FiniteDifferenceImageFilter<TInputImage, TOutputImage>
{
public:
  /**
   * Standard itk Self & Superclass typedefs
   */
  typedef SparseFieldLevelSetImageFilter Self;
  typedef FiniteDifferenceImageFilter<TInputImage, TOutputImage> Superclass;

  typedef typename Superclass::InputImageType  InputImageType;
  typedef typename Superclass::OutputImageType OutputImageType;
  typedef typename Superclass::FiniteDifferenceFunctionType
   FiniteDifferenceFunctionType;

  typedef typename InputImageType::IndexType  IndexType;

  /**
   * Dimensionality of input and output data is assumed to be the same.
   * It is inherited from the superclass.
   */
  enum { ImageDimension = Superclass::ImageDimension };

  /**
   * The pixel type of the output image will be used in computations.
   * Inherited from the superclass.
   */
  typedef typename Superclass::PixelType PixelType;

  /**
   * The value type of a time step.  Inherited from the superclass.
   */
  typedef typename Superclass::TimeStepType TimeStepType;

  /** 
   * Smart pointer support for this class.
   */
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;


  /**
   * Some typedefs specific for this class
   */
  //typedef ImageIndexValue<PixelType, ImageDimension>   LevelSetNodeType;

  typedef SparseLevelSetNode<PixelType, ImageDimension> LevelSetNodeType;
  typedef std::list<LevelSetNodeType *> LevelSetNodeListType;
  typedef typename LevelSetNodeListType::iterator LevelSetNodeListIteratorType;
  
  typedef Image<unsigned int, ImageDimension>   ByteImageType;
  

  /**
   *  Set the maximum number of nodes needed to pre-allocated
   */
  void SetMaxPreAllocateNodes(long int num)
  {
    m_MaxPreAllocateNodes = num;
  }

  /**
   *  Get the maximum number of nodes needed to pre-allocated
   */
  void GetMaxPreAllocateNodes(long int num)
  {
    return m_MaxPreAllocateNodes;
  }

  /**
   * Set the number of inside/outside layers
   */
  void SetNumberOfLayers(int num)
    {
      this->m_NumberOfLayers = num;
    }

  /**
   * Get the number of inside/outside layers
   */
  int GetNumberOfLayers(void)
    {
      return this->m_NumberOfLayers;
    }

  /**
   * Set the iso-surface value
   */
  void SetIsoValue(int num)
    {
      this->m_IsoValue = num;
    }

  /**
   * Run-time type information (and related methods)
   */
  itkTypeMacro(SparseFieldLevelSetImageFilter, ImageToImageFilter );
  
  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);

protected:
  SparseFieldLevelSetImageFilter()
    {
      m_NumberOfLayers = 4;
      m_MaxPreAllocateNodes = 0;
      m_IsoValue = 0;
      m_StatusImage = OutputImageType::New();
      
     } 

  ~SparseFieldLevelSetImageFilter() {}
  SparseFieldLevelSetImageFilter(const Self&) {}
  void operator=(const Self&) {}
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  /**
   * Structure for passing information into static callback methods.  Used in
   * the subclasses' threading mechanisms.
   */
  struct SparseFieldLevelSetThreadStruct
  {
    SparseFieldLevelSetImageFilter *Filter;
    TimeStepType TimeStep;
    TimeStepType *TimeStepList;
    bool *ValidTimeStepList;
  };
  
  /**
   * The type of region used for multithreading
   */
  typedef typename OutputImageType::RegionType ThreadRegionType;

  /**
   *  Allocate updatebuffer that might be used during calculate_change.
   *  Here it is a dummy function.
   */
  void AllocateUpdateBuffer(){ }
  
  
  /**
   * This method applies changes from the m_ActiveLists to the output and 
   * update all the lists  using the ThreadedApplyUpdate() method and a 
   * multithreading mechanism.  "dt" is  the time step to use for the 
   * update of each pixel. 
   */
  virtual void ApplyUpdate(TimeStepType dt);

  /**
   * This callback method uses ImageSource::SplitRequestedRegion to acquire an
   * output region that it passes to ThreadedApplyUpdate for processing.
   */
  static ITK_THREAD_RETURN_TYPE ApplyUpdateThreaderCallback( void *arg );
  
  /**
   * This method populates an update buffer with changes for each pixel in the
   * output using the ThreadedCalculateChange() method and a multithreading
   * mechanism. Returns value is a time step to be used for the update.
   */
  virtual TimeStepType CalculateChange();

  /**
   * This callback method uses SplitUpdateContainer to acquire a region
   * which it then passes to ThreadedCalculateChange for processing.
   */
  static ITK_THREAD_RETURN_TYPE CalculateChangeThreaderCallback( void *arg );
  
  /**
   * Split the UpdateBuffer into "num" pieces, returning region "i" as
   * "splitRegion". This method is called "num" times to return non-overlapping
   * regions. The method returns the number of pieces that the UpdateBuffer
   * can be split into by the routine. i.e. return value is less than or equal
   * to "num".
   * \sa ImageSource
   */
  //  virtual
  //  int SplitUpdateContainer(int i, int num, ThreadRegionType& splitRegion);

  /**
   *  Does the actual work of updating the output from the UpdateContainer over
   *  an output region supplied by the multithreading mechanism.
   *  \sa ApplyUpdate
   *  \sa ApplyUpdateThreaderCallback
   */ 
  virtual
  void ThreadedApplyUpdate(TimeStepType dt,
                           const ThreadRegionType &regionToProcess,
                           int threadId);
  // FOR ALL: iterator(output, splitRegion), iterator(update, splitRegion)

  /**
   * Does the actual work of calculating change over a region supplied by
   * the multithreading mechanism.
   * \sa CalculateChange
   * \sa CalculateChangeThreaderCallback
   */
  virtual
  TimeStepType ThreadedCalculateChange(const ThreadRegionType &regionToProcess,
                                       int threadId);
  // FOR ALL : iterator(input, splitRegion), iterator(update, splitRegion)


  /* 
   * This does initialization for the node lists and pre-allocate memory
   * for the free lists which would be used when memory allocation necessary
   * in updating the lists.   
   */  
  virtual void Initialize() ;

  /**
   * this routine assumes that input image has been set to some
   *  inside-outside function, and then it sets the values of the
   * active pixels and also constructs the list of active, inside, 
   * and outside pixels
   */
  virtual void ConstructLists();


  /**
   * Judge which thread region the pixel belongs to
   */
  int  GetThreadNum(IndexType index);

  /**
   *  Judge if a pixel is on the boundary
   */
  int  IsOnBoundary(IndexType index);

  /**
   *  Get the status of the layers
   */
  int GetStatus(int layer, int index)
    {
      if (layer == INSIDE) return ACTIVE_STATUS * (index+1) +1;
      else if(layer == OUTSIDE) return ACTIVE_STATUS * (index+1) -1;
      else return ACTIVE_STATUS;
    }

  /**
   *  Get A free memory unit from the free memory pool of each thread
   */
  LevelSetNodeType *  GetFreeItem(int threadId);


  /**
   * The buffer that stores the status information of the image.
   */

   typename OutputImageType::Pointer  m_StatusImage;
   

   /*
    * Declare the variable node lists on different layers of the interested 
    * levelsets
    */

  //This is the overall list over the whole requested region

  LevelSetNodeListType * m_ActiveList;
  LevelSetNodeListType * m_InsideList;
  LevelSetNodeListType * m_OutsideList;
  //  LevelSetNodeListType * m_BoundaryActiveList;

  // These are sublists for each thread region
  LevelSetNodeListType * m_ActiveLists;
  //LevelSetNodeListType * m_BourdaryActiveLists;
  LevelSetNodeListType ** m_InsideLists;
  LevelSetNodeListType ** m_OutsideLists;

  /**
   * This is the pre-allocated memory pool for each thread
   */
  LevelSetNodeListType * m_FreeLists;
  
  //These lists are used in updating the lists for each thread
  LevelSetNodeListType ** m_StatusUpLists;
  LevelSetNodeListType ** m_StatusDownLists;

  /**
   *  This defines the amount of memory to be pre-allocated 
   */
  long int m_MaxPreAllocateNodes;
  
  /**
   *  The number of inside/outside layers. The number of inside layers and 
   *  outside layers are the same
   */
  int m_NumberOfLayers;

  /**
   *  The status array of the layers
   *  
   */
  int * m_InStatus;
  int * m_OutStatus;

  /**
   *  The iso-surface we want to extract from the image data
   *  
   */

  PixelType m_IsoValue;

  /**
   *  The value of the pixel/voxels beyond the active layer and its 
   *  neighbored layers.
   *  
   */
  float OUTSIDE_VALUE;



  /**
   *  Some constant variables
   */
  static  int ACTIVE_STATUS;
  static  int CHANGING_STATUS;
  static  int UP_STATUS;
  static  int DOWN_STATUS;
  static  int INSIDE;
  static  int OUTSIDE;
  static  int ACTIVE;

  static  float DIFFERENCE_FACTOR;
  static  float CHANGE_FACTOR;


};
  

}// end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSparseFieldLevelSetImageFilter.txx"
#endif

#endif
