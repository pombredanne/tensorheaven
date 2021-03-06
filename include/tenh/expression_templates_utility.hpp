// ///////////////////////////////////////////////////////////////////////////
// tenh/expression_templates_utility.hpp by Victor Dods, created 2013/03/20
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_EXPRESSION_TEMPLATES_UTILITY_HPP_
#define TENH_EXPRESSION_TEMPLATES_UTILITY_HPP_

#include "tenh/core.hpp"

#include "tenh/componentindex.hpp"
#include "tenh/conceptual/abstractindex.hpp"
#include "tenh/conceptual/embeddableintensorproduct.hpp"
#include "tenh/conceptual/linearembedding.hpp"
#include "tenh/conceptual/tensorproduct.hpp"
#include "tenh/dimindex.hpp"
#include "tenh/implementation/implementationof.hpp"
#include "tenh/interface/expressiontemplate.hpp"
#include "tenh/multiindex.hpp"

namespace Tenh {

// this file contains template metaprograms which assist in the construction and evaluation of expression templates

// for this to work correctly on DimIndex_t types, the dimensions must be correct (i.e. the
// primal/dual vector space checking must already be done).  TODO: redesign-away this caveat
template <typename IndexTyple_>
struct SummedIndexTyple_f
{
    typedef typename ElementsHavingMultiplicity_f<IndexTyple_,2>::T T;
private:
    SummedIndexTyple_f();
};

// for this to work correctly on DimIndex_t types, the dimensions must be correct (i.e. the
// primal/dual vector space checking must already be done).  TODO: redesign-away this caveat
template <typename IndexTyple_>
struct FreeIndexTyple_f
{
    typedef typename ElementsHavingMultiplicity_f<IndexTyple_,1>::T T;
private:
    FreeIndexTyple_f();
};

// TODO: make into "raw" metafunction and make evaluator using macro
template <typename FreeIndexTyple_>
struct IndexIsFree_e
{
    static_assert(IsTyple_f<FreeIndexTyple_>::V, "FreeIndexTyple_ must be a Typle_t");

    template <typename T_>
    struct Eval_f
    {
        static_assert(IsTyple_f<T_>::V, "template argument must be a Typle_t");
        static_assert(Length_f<T_>::V == 2, "length of Typle_t argument must be exactly 2");
        // typedef typename Element_f<T_,0>::T Factor;
        typedef typename Element_f<T_,1>::T Index;
        static bool const V = Contains_f<FreeIndexTyple_,Index>::V;
    };
    template <typename T_>
    typename Eval_f<T_>::T operator () (T_ const &)
    {
        return typename Eval_f<T_>::T();
    }
private:
    IndexIsFree_e();
};

template <typename Unzipped_>
struct FreeFactorTypleHelper_f
{
    typedef typename Head_f<Unzipped_>::T T;
private:
    FreeFactorTypleHelper_f();
};

template <>
struct FreeFactorTypleHelper_f<Typle_t<>>
{
    typedef Typle_t<> T;
private:
    FreeFactorTypleHelper_f();
};

// for this to work correctly on DimIndex_t types, the dimensions must be correct (i.e. the
// primal/dual vector space checking must already be done).  TODO: redesign-away this caveat
template <typename FactorTyple_, typename IndexTyple_>
struct FreeFactorTyple_f
{
private:
    typedef typename Zip_f<Typle_t<FactorTyple_,IndexTyple_>>::T FactorAndIndexPairTyple;
    typedef typename FreeIndexTyple_f<IndexTyple_>::T FreeIndexTyple;
    typedef typename ElementsSatisfying_f<FactorAndIndexPairTyple,IndexIsFree_e<FreeIndexTyple>>::T FreeFactorAndIndexPairTyple;
    typedef typename Unzip_f<FreeFactorAndIndexPairTyple>::T Unzipped;
    static_assert(Length_f<Unzipped>::V == 0 || Length_f<Unzipped>::V == 2, "length must be exactly 0 or 2");
    FreeFactorTyple_f();
public:
    // each pair in FreeFactorAndIndexPairTyple is a (factor, index) Typle_t
    typedef typename FreeFactorTypleHelper_f<Unzipped>::T T;
};

template <typename AbstractIndexTyple_, typename SummedAbstractIndexTyple_, typename AbstractIndex>
struct SummedAbstractIndexPairElementIndices_f
{
    static_assert(IsAbstractIndex_f<AbstractIndex>::V, "AbstractIndex must be an AbstractIndex_c");
    static_assert(OccurrenceCount_f<AbstractIndexTyple_,AbstractIndex>::V == 2, "AbstractIndex must occur exactly twice in AbstractIndexTyple_");
    static Uint32 const FIRST = IndexOfFirstOccurrence_f<AbstractIndexTyple_,AbstractIndex>::V;
private:
    SummedAbstractIndexPairElementIndices_f();
    static Uint32 const FIRST_MATCHING_IN_REST = IndexOfFirstOccurrence_f<typename TrailingTyple_f<AbstractIndexTyple_,FIRST+1>::T,AbstractIndex>::V;
public:
    static Uint32 const SECOND = FIRST + 1 + FIRST_MATCHING_IN_REST;
};

template <typename FirstFactor, typename SecondFactor>
struct SummationIsNaturalPairing_f
{
    static bool const V = TypesAreEqual_f<FirstFactor,typename DualOf_f<SecondFactor>::T>::V;
private:
    SummationIsNaturalPairing_f();
};

template <typename FactorTyple, typename AbstractIndexTyple, typename SummedAbstractIndexTyple, typename AbstractIndex>
struct FactorsOfSummation_f
{
    typedef SummedAbstractIndexPairElementIndices_f<AbstractIndexTyple,SummedAbstractIndexTyple,AbstractIndex> Pair;
    typedef typename Element_f<FactorTyple,Pair::FIRST>::T FirstFactor;
    typedef typename Element_f<FactorTyple,Pair::SECOND>::T SecondFactor;
private:
    static_assert(Length_f<FactorTyple>::V == Length_f<AbstractIndexTyple>::V, "FactorTyple and AbstractIndexTyple must have equal lengths");
    static_assert(HasBasedVectorSpaceStructure_f<FirstFactor>::V, "FirstFactor must have unique based vector space structure");
    static_assert(HasBasedVectorSpaceStructure_f<SecondFactor>::V, "SecondFactor must have unique based vector space structure");
    FactorsOfSummation_f();
};

template <typename FactorTyple, typename AbstractIndexTyple, typename SummedAbstractIndexTyple>
struct AllSummationsAreNaturalPairings_f
{
private:
    typedef FactorsOfSummation_f<FactorTyple,
                                 AbstractIndexTyple,
                                 SummedAbstractIndexTyple,
                                 typename Head_f<SummedAbstractIndexTyple>::T> FactorsOfSummation;
    static bool const V_HEAD = SummationIsNaturalPairing_f<typename FactorsOfSummation::FirstFactor,
                                                           typename FactorsOfSummation::SecondFactor>::V;
    static bool const V_BODY = AllSummationsAreNaturalPairings_f<FactorTyple,
                                                                 AbstractIndexTyple,
                                                                 typename BodyTyple_f<SummedAbstractIndexTyple>::T>::V;
    AllSummationsAreNaturalPairings_f();
public:
    static bool const V = V_HEAD && V_BODY;
};

template <typename FactorTyple, typename AbstractIndexTyple>
struct AllSummationsAreNaturalPairings_f<FactorTyple,AbstractIndexTyple,Typle_t<>>
{
    static bool const V = true; // so this can be used in static_assert
private:
    AllSummationsAreNaturalPairings_f();
};

// this is designed to handle trace-type expression templates, such as u(i,i) or v(i,j,i)
// technically SummedDimIndexTyple is a redundant argument (as it is derivable from TensorDimIndexTyple),
// but it is necessary so that a template specialization can be made for when it is Typle_t<>.
template <typename Tensor, typename TensorDimIndexTyple, typename SummedDimIndexTyple>
struct UnarySummation_t
{
private:

    typedef typename AbstractIndicesOfDimIndexTyple_f<TensorDimIndexTyple>::T AbstractIndexTyple;
    typedef typename AbstractIndicesOfDimIndexTyple_f<SummedDimIndexTyple>::T SummedAbstractIndexTyple;
    static_assert(AllSummationsAreNaturalPairings_f<typename Tensor::FactorTyple,
                                                    AbstractIndexTyple,
                                                    SummedAbstractIndexTyple>::V, "all summations must be natural pairings");

public:

    typedef typename Tensor::Scalar Scalar;
    typedef typename FreeIndexTyple_f<TensorDimIndexTyple>::T FreeDimIndexTyple;
    typedef MultiIndex_t<FreeDimIndexTyple> MultiIndex;

    static Scalar eval (Tensor const &tensor, MultiIndex const &m)
    {
        typedef typename Concat2Typles_f<FreeDimIndexTyple,SummedDimIndexTyple>::T TotalDimIndexTyple;
        typedef MultiIndex_t<TotalDimIndexTyple> TotalMultiIndex;
        typedef MultiIndex_t<SummedDimIndexTyple> SummedMultiIndex;

        // the operands take indices that are a subset of the summed indices and free indices.

        // constructing t with m initializes the first elements which correpond to
        // MultiIndex with the value of m, and initializes the remaining elements to zero.
        TotalMultiIndex t(m);
        Scalar retval(0);
        // get the map which produces the MultiIndex for each tensor from the TotalMultiIndex t
        typedef MultiIndexMap_t<TotalDimIndexTyple,TensorDimIndexTyple> TensorIndexMap;
        static typename TensorIndexMap::EvalMapType const tensor_index_map = TensorIndexMap::eval;
        // t = (f,s), which is a concatenation of the free access indices and the summed access indices.
        // s is a reference to the second part, which is what is iterated over in the summation.
        for (SummedMultiIndex &s = t.template trailing_tuple<Length_f<FreeDimIndexTyple>::V>(); s.is_not_at_end(); ++s)
            // TODO: when the dual-vector-space/conceptual refactor is done, this summation_component_factor
            // should go away, since this is a non-natural pairing, and it causes C++ plumbing issues
            // (getting the C++ scalar type from the index, where the index will only be aware of the
            // abstract BasedVectorSpace).
            retval += tensor[tensor_index_map(t)];// * summation_component_factor(s);
        return retval;
    }
};

template <typename Tensor, typename TensorDimIndexTyple>
struct UnarySummation_t<Tensor,TensorDimIndexTyple,Typle_t<>>
{
    // no summations to check the natural pairing for

    typedef typename Tensor::Scalar Scalar;
    typedef typename FreeIndexTyple_f<TensorDimIndexTyple>::T FreeDimIndexTyple;
    typedef MultiIndex_t<FreeDimIndexTyple> MultiIndex;

    static Scalar eval (Tensor const &tensor, MultiIndex const &m) { return tensor[m]; }
};

template <typename LeftOperand, typename RightOperand, typename FreeDimIndexTyple, typename SummedDimIndexTyple>
struct BinarySummation_t
{
private:
    typedef typename Concat2Typles_f<typename LeftOperand::FreeFactorTyple,
                                     typename RightOperand::FreeFactorTyple>::T FactorTyple;
    typedef typename Concat2Typles_f<typename LeftOperand::FreeDimIndexTyple,
                                     typename RightOperand::FreeDimIndexTyple>::T DimIndexTyple;
    typedef typename AbstractIndicesOfDimIndexTyple_f<DimIndexTyple>::T AbstractIndexTyple;
    typedef typename AbstractIndicesOfDimIndexTyple_f<SummedDimIndexTyple>::T SummedAbstractIndexTyple;
    static_assert(IsExpressionTemplate_f<LeftOperand>::V, "LeftOperand must be an ExpressionTemplate_i");
    static_assert(IsExpressionTemplate_f<RightOperand>::V, "RightOperand must be an ExpressionTemplate_i");
    static_assert(TypesAreEqual_f<typename LeftOperand::Scalar,typename RightOperand::Scalar>::V, "operand scalar types must be equal");
    static_assert(Length_f<SummedDimIndexTyple>::V > 0, "number of summed indices must be positive");
    static_assert(AllSummationsAreNaturalPairings_f<FactorTyple,
                                                    AbstractIndexTyple,
                                                    SummedAbstractIndexTyple>::V, "all summations must be natural pairings");
public:
    typedef typename LeftOperand::Scalar Scalar;
    typedef MultiIndex_t<FreeDimIndexTyple> MultiIndex;

    static Scalar eval (LeftOperand const &left_operand, RightOperand const &right_operand, MultiIndex const &m)
    {
        typedef typename Concat2Typles_f<FreeDimIndexTyple,SummedDimIndexTyple>::T TotalDimIndexTyple;
        typedef MultiIndex_t<TotalDimIndexTyple> TotalMultiIndex;
        typedef MultiIndex_t<SummedDimIndexTyple> SummedMultiIndex;

        // the operands take indices that are a subset of the summed indices and free indices.

        // constructing t with m initializes the first elements which correpond to
        // MultiIndex with the value of m, and initializes the remaining elements to zero.
        TotalMultiIndex t(m);
        Scalar retval(0);
        // get the map which produces the MultiIndex for each operand from the TotalMultiIndex t
        typedef MultiIndexMap_t<TotalDimIndexTyple,typename LeftOperand::FreeDimIndexTyple> LeftOperandIndexMap;
        typedef MultiIndexMap_t<TotalDimIndexTyple,typename RightOperand::FreeDimIndexTyple> RightOperandIndexMap;
        static typename LeftOperandIndexMap::EvalMapType const left_operand_index_map = LeftOperandIndexMap::eval;
        static typename RightOperandIndexMap::EvalMapType const right_operand_index_map = RightOperandIndexMap::eval;
        // t = (f,s), which is a concatenation of the free access indices and the summed access indices.
        // s is a reference to the second part, which is what is iterated over in the summation.
        for (SummedMultiIndex &s = t.template trailing_tuple<Length_f<FreeDimIndexTyple>::V>(); s.is_not_at_end(); ++s)
            retval += left_operand[left_operand_index_map(t)] *
                      right_operand[right_operand_index_map(t)];// *
                      //summation_component_factor(s);
        return retval;
    }
};

// template specialization handles summation over no indices
template <typename LeftOperand, typename RightOperand, typename FreeDimIndexTyple>
struct BinarySummation_t<LeftOperand,RightOperand,FreeDimIndexTyple,Typle_t<>>
{
    static_assert(IsExpressionTemplate_f<LeftOperand>::V, "LeftOperand must be an ExpressionTemplate_i");
    static_assert(IsExpressionTemplate_f<RightOperand>::V, "RightOperand must be an ExpressionTemplate_i");
    static_assert(TypesAreEqual_f<typename LeftOperand::Scalar,typename RightOperand::Scalar>::V, "operand scalar types must be equal");
    static_assert(EachTypeSatisfies_f<FreeDimIndexTyple,IsDimIndex_e>::V, "each free index must be a DimIndex_t");
    // no summation, so no need to check naturality of pairings

    typedef typename LeftOperand::Scalar Scalar;
    typedef MultiIndex_t<FreeDimIndexTyple> MultiIndex;

    static Scalar eval (LeftOperand const &left_operand, RightOperand const &right_operand, MultiIndex const &m)
    {
        // get the map which produces the MultiIndex for each operand from the free indices m
        typedef MultiIndexMap_t<FreeDimIndexTyple,typename LeftOperand::FreeDimIndexTyple> LeftOperandIndexMap;
        typedef MultiIndexMap_t<FreeDimIndexTyple,typename RightOperand::FreeDimIndexTyple> RightOperandIndexMap;
        static typename LeftOperandIndexMap::EvalMapType const left_operand_index_map = LeftOperandIndexMap::eval;
        static typename RightOperandIndexMap::EvalMapType const right_operand_index_map = RightOperandIndexMap::eval;
        return left_operand[left_operand_index_map(m)] * right_operand[right_operand_index_map(m)];
    }
};


template <typename LeftOperand, typename RightOperand>
struct FreeDimIndexTypleOfMultiplication_f
{
private:
    // the free indices are the single-occurrence indices of the concatenated
    // typle of free indices from the left and right operands
    typedef typename Concat2Typles_f<typename LeftOperand::FreeDimIndexTyple,
                                     typename RightOperand::FreeDimIndexTyple>::T CombinedFreeDimIndexTyple;
    FreeDimIndexTypleOfMultiplication_f();
public:
    typedef typename ElementsHavingMultiplicity_f<CombinedFreeDimIndexTyple,1>::T T;
};

template <typename LeftOperand, typename RightOperand>
struct SummedDimIndexTypleOfMultiplication_f
{
private:
    // the free indices are the single-occurrence indices of the concatenated
    // typle of free indices from the left and right operands
    typedef typename Concat2Typles_f<typename LeftOperand::FreeDimIndexTyple,
                                     typename RightOperand::FreeDimIndexTyple>::T CombinedFreeDimIndexTyple;
    SummedDimIndexTypleOfMultiplication_f();
public:
    // the summed indices (at this level) are the double-occurrences indices
    // of the concatenated typle of free indices from the left and right operands
    typedef typename ElementsHavingMultiplicity_f<CombinedFreeDimIndexTyple,2>::T T;
};

template <typename LeftOperand, typename RightOperand>
struct UsedDimIndexTypleOfMultiplication_f
{
private:
    typedef typename SummedDimIndexTypleOfMultiplication_f<LeftOperand,RightOperand>::T SummedDimIndexTyple;
    UsedDimIndexTypleOfMultiplication_f();
public:
    // typle of used indices which are prohibited from using higher up in the AST
    typedef typename UniqueTypesIn_f<
        typename ConcatTyples_f<
            typename LeftOperand::UsedDimIndexTyple,
            typename RightOperand::UsedDimIndexTyple,
            SummedDimIndexTyple>::T>::T T;
};

template <typename LeftOperand, typename RightOperand>
struct FreeFactorTypleOfMultiplication_f
{
private:
    typedef typename Concat2Typles_f<typename LeftOperand::FreeFactorTyple,
                                     typename RightOperand::FreeFactorTyple>::T CombinedFactorTyple;
    typedef typename Concat2Typles_f<typename LeftOperand::FreeDimIndexTyple,
                                     typename RightOperand::FreeDimIndexTyple>::T CombinedDimIndexTyple;
    FreeFactorTypleOfMultiplication_f();
public:
    typedef typename FreeFactorTyple_f<CombinedFactorTyple,CombinedDimIndexTyple>::T T;
};



template <typename AbstractIndexTyple, typename FactorTyple, typename ExtractionAbstractIndexTyple>
struct ExtractFactorsForAbstractIndices_f
{
private:
    static_assert(OccurrenceCount_f<AbstractIndexTyple,typename Head_f<ExtractionAbstractIndexTyple>::T>::V == 1, "each extracted type must occur exactly once in AbstractIndexTyple");
    static_assert(IsASubsetOf_f<ExtractionAbstractIndexTyple,AbstractIndexTyple>::V, "ExtractionAbstractIndexTyple must be a subset of AbstractIndexTyple");
    static Uint32 const INDEX = IndexOfFirstOccurrence_f<AbstractIndexTyple,typename Head_f<ExtractionAbstractIndexTyple>::T>::V;
    ExtractFactorsForAbstractIndices_f();
public:
    typedef typename HeadBodyTyple_f<typename Element_f<FactorTyple,INDEX>::T,
                                     typename ExtractFactorsForAbstractIndices_f<AbstractIndexTyple,
                                                                                 FactorTyple,
                                                                                 typename BodyTyple_f<ExtractionAbstractIndexTyple>::T>::T>::T T;
};

template <typename AbstractIndexTyple, typename FactorTyple>
struct ExtractFactorsForAbstractIndices_f<AbstractIndexTyple,FactorTyple,Typle_t<>>
{
    typedef Typle_t<> T;
private:
    ExtractFactorsForAbstractIndices_f();
};

// TODO: Scalar will go away once the bundle map is moved into the conceptual layer.
template <typename Scalar, typename BundleDimIndexTyple, typename ResultingFactorType, typename ResultingDimIndexType>
struct BundleIndexMap_t
{
    typedef MultiIndex_t<BundleDimIndexTyple> (*T) (ResultingDimIndexType const &);
    static T const V;
private:
    BundleIndexMap_t();
};

// TODO: the use of UseMemberArray_t<ComponentsAreConst::FALSE> is somewhat arbitrary -- should this be addressed somehow?
template <typename Scalar, typename BundleDimIndexTyple, typename ResultingFactorType, typename ResultingDimIndexType>
typename BundleIndexMap_t<Scalar,BundleDimIndexTyple,ResultingFactorType,ResultingDimIndexType>::T const BundleIndexMap_t<Scalar,BundleDimIndexTyple,ResultingFactorType,ResultingDimIndexType>::V =
    ImplementationOf_t<ResultingFactorType,Scalar,UseMemberArray_t<ComponentsAreConst::FALSE>>::template bundle_index_map<BundleDimIndexTyple,ResultingDimIndexType>;

// not an expression template, but just something that handles the bundled indices
template <typename Operand, typename BundleAbstractIndexTyple, typename ResultingFactorType, typename ResultingAbstractIndexType, CheckFactorTypes CHECK_FACTOR_TYPES_>
struct IndexBundle_t
{
    typedef typename AbstractIndicesOfDimIndexTyple_f<typename Operand::FreeDimIndexTyple>::T OperandFreeAbstractIndexTyple;

    static_assert(IsExpressionTemplate_f<Operand>::V, "Operand must be an ExpressionTemplate_i");
    static_assert(IS_EMBEDDABLE_IN_TENSOR_PRODUCT_OF_VECTOR_SPACES_UNIQUELY(ResultingFactorType), "ResultingFactorType must have unique embeddable in tensor product of based vector spaces structure");
    static_assert(IsAbstractIndex_f<ResultingAbstractIndexType>::V, "ResultingAbstractIndexType must be an AbstractIndex_c");
    static_assert(IsASubsetOf_f<BundleAbstractIndexTyple,OperandFreeAbstractIndexTyple>::V, "bundled indices must be a subset of the free indices");
    static_assert(!Contains_f<BundleAbstractIndexTyple,ResultingAbstractIndexType>::V, "BundleAbstractIndexTyple must not contain ResultingAbstractIndexType");

    // if Operand's free indices are i*j*k*l*m*n, the bundle indices are i*k*l, and the resulting
    // index is P, then the transformation is
    //      i*j*k*l*m*n --> j*m*n*i*k*l --> j*m*n*P
    // the first transformation being putting the bundle indices at the back (preserving their order),
    // and the second one transforming them into the bundled index.  the component access goes backward
    // in this diagram (components are accessed via the indices j*m*n*P, the bundle map is applied to
    // P to get i*k*l, and then j*m*n*i*k*l is transformed to i*j*k*l*m*n to get the component from
    // Operand).

    typedef typename DimIndexTypleOf_f<typename Operand::FreeFactorTyple,
                                       OperandFreeAbstractIndexTyple>::T OperandFreeDimIndexTyple;
    typedef typename ExtractFactorsForAbstractIndices_f<OperandFreeAbstractIndexTyple,
                                                        typename Operand::FreeFactorTyple,
                                                        BundleAbstractIndexTyple>::T BundleFactorTyple;
    typedef typename DimIndexTypleOf_f<BundleFactorTyple,
                                       BundleAbstractIndexTyple>::T BundleDimIndexTyple;
    typedef DimIndex_t<ResultingAbstractIndexType::SYMBOL,
                       DimensionOf_f<ResultingFactorType>::V> ResultingDimIndexType;

private:

    typedef typename FactorTypleOf_f<typename AS_EMBEDDABLE_IN_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES(ResultingFactorType)::TensorProductOfBasedVectorSpaces>::T ResultingFactorTypeFactorTyple;
    static_assert(!bool(CHECK_FACTOR_TYPES_) || TypesAreEqual_f<BundleFactorTyple,ResultingFactorTypeFactorTyple>::V, "bundle factors must match");

public:

    // zip the stuff so that the transformations can act on both the DimIndex_t and factor typles
    typedef typename Zip_f<Typle_t<OperandFreeDimIndexTyple,
                                   typename Operand::FreeFactorTyple>>::T OperandFreeDimIndexAndFactorTyple;
    typedef Typle_t<ResultingDimIndexType,ResultingFactorType> ResultingDimIndexAndFactorType;
    typedef typename Zip_f<Typle_t<BundleDimIndexTyple,
                                   BundleFactorTyple>>::T BundleDimIndexAndFactorTyple;

    // ResultingDimIndexAndFactorType comes last.  DimIndexAndFactorTyple may have summed indices
    typedef typename SetSubtraction_f<
        typename Concat2Typles_f<OperandFreeDimIndexAndFactorTyple,
                                        Typle_t<ResultingDimIndexAndFactorType>>::T,
        BundleDimIndexAndFactorTyple>::T DimIndexAndFactorTyple;
    // this seems like it should be the same as OperandFreeDimIndexAndFactorTyple, but it has
    // the indices in BundleDimIndexAndFactorTyple coming last.
    typedef typename Concat2Typles_f<
        typename SetSubtraction_f<OperandFreeDimIndexAndFactorTyple,BundleDimIndexAndFactorTyple>::T,
        BundleDimIndexAndFactorTyple>::T UnpackedDimIndexAndFactorTyple;

    // unzip the stuff
    typedef typename Unzip_f<DimIndexAndFactorTyple>::T DimIndexAndFactorTyple_Unzipped;
    typedef typename Unzip_f<UnpackedDimIndexAndFactorTyple>::T UnpackedDimIndexAndFactorTyple_Unzipped;
    typedef typename Element_f<DimIndexAndFactorTyple_Unzipped,0>::T DimIndexTyple;
    typedef typename Element_f<DimIndexAndFactorTyple_Unzipped,1>::T FactorTyple;
    typedef typename Head_f<UnpackedDimIndexAndFactorTyple_Unzipped>::T UnpackedDimIndexTyple;

    // finally, define the multi-index and bundle index map.
    typedef MultiIndex_t<DimIndexTyple> MultiIndex;
    typedef typename Operand::Scalar Scalar;
    typedef BundleIndexMap_t<Scalar,
                             BundleDimIndexTyple,
                             ResultingFactorType,
                             ResultingDimIndexType> BundleIndexMap;

    IndexBundle_t (Operand const &operand) : m_operand(operand) { }

    Scalar operator [] (MultiIndex const &m) const
    {
        // replace the head of m with the separate indices that it bundles.
        // use MultiIndexMap_t to place the indices in the correct order.
        typedef MultiIndexMap_t<UnpackedDimIndexTyple,typename Operand::FreeDimIndexTyple> OperandIndexMap;
        static typename OperandIndexMap::EvalMapType const operand_index_map = OperandIndexMap::eval;
        static typename BundleIndexMap::T const bundle_index_map = BundleIndexMap::V;
        // | is concatenation of MultiIndex_t instances
        return m_operand[operand_index_map(m.template leading_tuple<MultiIndex::LENGTH-1>()
                                           |
                                           bundle_index_map(m.template el<MultiIndex::LENGTH-1>()))];
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_operand.overlaps_memory_range(ptr, range);
    }

    Operand const &operand () const { return m_operand; }

private:

    void operator = (IndexBundle_t const &);

    Operand m_operand;
};

// not an expression template, but just something that handles the split indices
template <typename Operand, typename SourceAbstractIndexType, typename SplitAbstractIndexTyple>
struct IndexSplitter_t
{
    typedef typename AbstractIndicesOfDimIndexTyple_f<typename Operand::FreeDimIndexTyple>::T OperandFreeAbstractIndexTyple;

    static_assert(IsExpressionTemplate_f<Operand>::V, "Operand must be an ExpressionTemplate_i");
    static_assert(IsAbstractIndex_f<SourceAbstractIndexType>::V, "SourceAbstractIndexType must be an AbstractIndex_c");
    static_assert(Contains_f<OperandFreeAbstractIndexTyple,SourceAbstractIndexType>::V, "source index must be free");
    static_assert(!HasNontrivialSetIntersection_f<Typle_t<SourceAbstractIndexType>,SplitAbstractIndexTyple>::V, "source and split indices must be distinct");

    typedef typename Operand::Scalar Scalar;
    // we must replace SourceAbstractIndexType with MultiIndex_t<SplitAbstractIndexTyple> in the index typle
    static Uint32 const SOURCE_INDEX_TYPE_INDEX = IndexOfFirstOccurrence_f<OperandFreeAbstractIndexTyple,SourceAbstractIndexType>::V;
    typedef typename Element_f<typename Operand::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX>::T SourceFactor;

    typedef typename AS_EMBEDDABLE_IN_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES(SourceFactor)::TensorProductOfBasedVectorSpaces EmbeddingTensorProduct;

    static_assert(OrderOf_f<EmbeddingTensorProduct>::V == Length_f<SplitAbstractIndexTyple>::V, "order of EmbeddingTensorProduct and length of SplitAbstractIndexTyple must be equal");

    typedef typename Concat2Typles_f<
        typename LeadingTyple_f<typename Operand::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX>::T,
        typename Concat2Typles_f<
            typename FactorTypleOf_f<EmbeddingTensorProduct>::T,
            typename TrailingTyple_f<typename Operand::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX+1>::T
            >::T
        >::T FactorTyple;
    typedef typename Concat2Typles_f<
        typename LeadingTyple_f<typename Operand::FreeDimIndexTyple,SOURCE_INDEX_TYPE_INDEX>::T,
        typename Concat2Typles_f<
            SplitAbstractIndexTyple,
            typename TrailingTyple_f<typename Operand::FreeDimIndexTyple,SOURCE_INDEX_TYPE_INDEX+1>::T
            >::T
        >::T AbstractIndexTyple;
    typedef typename DimIndexTypleOf_f<FactorTyple,AbstractIndexTyple>::T DimIndexTyple;
    typedef typename Concat2Typles_f<typename Operand::UsedDimIndexTyple,SplitAbstractIndexTyple>::T UsedDimIndexTyple;
    typedef MultiIndex_t<DimIndexTyple> MultiIndex;

    static_assert(Length_f<FactorTyple>::V == Length_f<DimIndexTyple>::V, "must have same number of factors and indices");

    IndexSplitter_t (Operand const &operand) : m_operand(operand) { }

    Scalar operator [] (MultiIndex const &m) const
    {
        typedef typename DimIndexTypleOf_f<typename FactorTypleOf_f<typename AS_EMBEDDABLE_IN_TENSOR_PRODUCT_OF_VECTOR_SPACES(SourceFactor)::TensorProductOfVectorSpaces>::T,
                                           SplitAbstractIndexTyple>::T SourceFactorDimIndexTyple;
        typedef ComponentIndex_t<DimensionOf_f<SourceFactor>::V> SourceFactorComponentIndex;
        typedef MultiIndex_t<SourceFactorDimIndexTyple> SourceFactorMultiIndex;
        // TODO: the use of UseMemberArray_t<ComponentsAreConst::FALSE> here is arbitrary because it's just used to access a
        // static method.  figure out if this is a problem
        typedef ImplementationOf_t<SourceFactor,Scalar,UseMemberArray_t<ComponentsAreConst::FALSE>> ImplementationOfSourceFactor;

        SourceFactorMultiIndex s(m.template range<SOURCE_INDEX_TYPE_INDEX,SOURCE_INDEX_TYPE_INDEX+Length_f<SplitAbstractIndexTyple>::V>());
        if (ImplementationOfSourceFactor::component_is_procedural_zero(s))
            return Scalar(0);

        SourceFactorComponentIndex i(ImplementationOfSourceFactor::vector_index_of(s));
        // this replaces the SplitAbstractIndexTyple portion with SourceAbstractIndexType
        typename Operand::MultiIndex c_rebundled(m.template leading_tuple<SOURCE_INDEX_TYPE_INDEX>()
                                                 |
                                                 (i >>= m.template trailing_tuple<SOURCE_INDEX_TYPE_INDEX+Length_f<SplitAbstractIndexTyple>::V>()));
        return ImplementationOfSourceFactor::scalar_factor_for_component(s) * m_operand[c_rebundled];
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_operand.overlaps_memory_range(ptr, range);
    }

    Operand const &operand () const { return m_operand; }

private:

    void operator = (IndexSplitter_t const &);

    Operand m_operand;
};

// not an expression template, but just something that handles the split indices
template <typename Operand, typename SourceAbstractIndexType, typename SplitAbstractIndexType>
struct IndexSplitToIndex_t
{
    typedef typename AbstractIndicesOfDimIndexTyple_f<typename Operand::FreeDimIndexTyple>::T OperandFreeAbstractIndexTyple;

    static_assert(IsExpressionTemplate_f<Operand>::V, "Operand must be an ExpressionTemplate_i");
    static_assert(IsAbstractIndex_f<SourceAbstractIndexType>::V, "SourceAbstractIndexType must be an AbstractIndex_c");
    static_assert(IsAbstractIndex_f<SplitAbstractIndexType>::V, "SplitAbstractIndexType must be an AbstractIndex_c");
    static_assert(Contains_f<OperandFreeAbstractIndexTyple,SourceAbstractIndexType>::V, "source index must be free");
    static_assert(!TypesAreEqual_f<SourceAbstractIndexType,SplitAbstractIndexType>::V, "source and split indices must be distinct");

    typedef typename Operand::Scalar Scalar;
    // we must replace SourceAbstractIndexType with SplitAbstractIndexType in the index typle
    static Uint32 const SOURCE_INDEX_TYPE_INDEX = IndexOfFirstOccurrence_f<OperandFreeAbstractIndexTyple,SourceAbstractIndexType>::V;
    typedef typename Element_f<typename Operand::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX>::T SourceFactor;

    typedef typename AS_EMBEDDABLE_IN_TENSOR_PRODUCT_OF_BASED_VECTOR_SPACES(SourceFactor)::TensorProductOfBasedVectorSpaces EmbeddingTensorProduct;
    typedef typename ConcatTyples_f<
        typename LeadingTyple_f<typename Operand::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX>::T,
        Typle_t<EmbeddingTensorProduct>,
        typename TrailingTyple_f<typename Operand::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX+1>::T>::T FactorTyple;
    typedef typename ConcatTyples_f<
        typename LeadingTyple_f<typename Operand::FreeDimIndexTyple,SOURCE_INDEX_TYPE_INDEX>::T,
        Typle_t<SplitAbstractIndexType>,
        typename TrailingTyple_f<typename Operand::FreeDimIndexTyple,SOURCE_INDEX_TYPE_INDEX+1>::T>::T AbstractIndexTyple;
    typedef typename DimIndexTypleOf_f<FactorTyple,AbstractIndexTyple>::T DimIndexTyple;
    typedef typename Concat2Typles_f<typename Operand::UsedDimIndexTyple,Typle_t<SplitAbstractIndexType>>::T UsedDimIndexTyple;
    typedef MultiIndex_t<DimIndexTyple> MultiIndex;

    static_assert(Length_f<FactorTyple>::V == Length_f<DimIndexTyple>::V, "must have same number of factors and indices");

    IndexSplitToIndex_t (Operand const &operand) : m_operand(operand) { }

    Scalar operator [] (MultiIndex const &m) const
    {
        typedef ComponentIndex_t<DimensionOf_f<SourceFactor>::V> SourceFactorComponentIndex;
        // TODO: the use of UseMemberArray_t<ComponentsAreConst::FALSE> here is arbitrary because it's just used to access a
        // static method.  figure out if this is a problem
        typedef ImplementationOf_t<SourceFactor,Scalar,UseMemberArray_t<ComponentsAreConst::FALSE>> ImplementationOfSourceFactor;
        typedef typename ImplementationOfSourceFactor::MultiIndex SourceFactorMultiIndex;

        // this does the vector-index to multi-index conversion
        SourceFactorMultiIndex s(m.template el<SOURCE_INDEX_TYPE_INDEX>());
        if (ImplementationOfSourceFactor::component_is_procedural_zero(s))
            return Scalar(0);

        SourceFactorComponentIndex i(ImplementationOfSourceFactor::vector_index_of(s));
        // this replaces the SplitAbstractIndexType_ portion with SourceAbstractIndexType
        typename Operand::MultiIndex c_rebundled(m.template leading_tuple<SOURCE_INDEX_TYPE_INDEX>()
                                                 |
                                                 (i >>= m.template trailing_tuple<SOURCE_INDEX_TYPE_INDEX+1>()));
        return ImplementationOfSourceFactor::scalar_factor_for_component(s) * m_operand[c_rebundled];
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_operand.overlaps_memory_range(ptr, range);
    }

    Operand const &operand () const { return m_operand; }

private:

    void operator = (IndexSplitToIndex_t const &);

    Operand m_operand;
};

// not an expression template, but just something that handles the embedded indices
template <typename Operand_,
          typename SourceAbstractIndexType_,
          typename EmbeddingCodomain_,
          typename EmbeddedAbstractIndexType_,
          typename EmbeddingId_>
struct IndexEmbedder_t
{
    typedef typename AbstractIndicesOfDimIndexTyple_f<typename Operand_::FreeDimIndexTyple>::T OperandFreeAbstractIndexTyple;

    static_assert(IsExpressionTemplate_f<Operand_>::V, "Operand_ must be an ExpressionTemplate_i");
    static_assert(IsAbstractIndex_f<SourceAbstractIndexType_>::V, "SourceAbstractIndexType_ must be an AbstractIndex_c");
    static_assert(IsAbstractIndex_f<EmbeddedAbstractIndexType_>::V, "EmbeddedAbstractIndexType_ must be an AbstractIndex_c");
    static_assert(Contains_f<OperandFreeAbstractIndexTyple,SourceAbstractIndexType_>::V, "source index must be free");
    static_assert(!TypesAreEqual_f<SourceAbstractIndexType_,EmbeddedAbstractIndexType_>::V, "source and embedded indices must be distinct");

    typedef typename Operand_::Scalar Scalar;
    // we must replace SourceAbstractIndexType_ with EmbeddedAbstractIndexType_ in the index typle
    static Uint32 const SOURCE_INDEX_TYPE_INDEX = IndexOfFirstOccurrence_f<OperandFreeAbstractIndexTyple,SourceAbstractIndexType_>::V;
    typedef typename Element_f<typename Operand_::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX>::T EmbeddingDomain;

    typedef typename ConcatTyples_f<
        typename LeadingTyple_f<typename Operand_::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX>::T,
        Typle_t<EmbeddingCodomain_>,
        typename TrailingTyple_f<typename Operand_::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX+1>::T>::T FactorTyple;
    typedef typename ConcatTyples_f<
        typename LeadingTyple_f<typename Operand_::FreeDimIndexTyple,SOURCE_INDEX_TYPE_INDEX>::T,
        Typle_t<EmbeddedAbstractIndexType_>,
        typename TrailingTyple_f<typename Operand_::FreeDimIndexTyple,SOURCE_INDEX_TYPE_INDEX+1>::T>::T AbstractIndexTyple;
    typedef typename DimIndexTypleOf_f<FactorTyple,AbstractIndexTyple>::T DimIndexTyple;
    typedef typename Concat2Typles_f<typename Operand_::UsedDimIndexTyple,Typle_t<EmbeddedAbstractIndexType_>>::T UsedDimIndexTyple;
    typedef MultiIndex_t<DimIndexTyple> MultiIndex;

    static_assert(Length_f<FactorTyple>::V == Length_f<DimIndexTyple>::V, "must have same number of factors and indices");

    IndexEmbedder_t (Operand_ const &operand) : m_operand(operand) { }

    Scalar operator [] (MultiIndex const &m) const
    {
        typedef ComponentIndex_t<DimensionOf_f<EmbeddingCodomain_>::V> EmbeddingCodomainComponentIndex;
        typedef ComponentIndex_t<DimensionOf_f<EmbeddingDomain>::V> EmbeddingDomainComponentIndex;

        EmbeddingCodomainComponentIndex j(m.template el<SOURCE_INDEX_TYPE_INDEX>());

        typedef typename LinearEmbedding_f<EmbeddingDomain,EmbeddingCodomain_,Scalar,EmbeddingId_,WithExceptions::DISABLED>::T LinearEmbedding;
        if (LinearEmbedding::embedded_component_is_procedural_zero(j))
            return Scalar(0);

        EmbeddingDomainComponentIndex i(LinearEmbedding::source_component_index_for_embedded_component(j));
        // this replaces the EmbeddedAbstractIndexType_ portion with SourceAbstractIndexType_
        typename Operand_::MultiIndex c_rebundled(m.template leading_tuple<SOURCE_INDEX_TYPE_INDEX>()
                                                 |
                                                 (i >>= m.template trailing_tuple<SOURCE_INDEX_TYPE_INDEX+1>()));
        return LinearEmbedding::scalar_factor_for_embedded_component(j) * m_operand[c_rebundled];
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_operand.overlaps_memory_range(ptr, range);
    }

    Operand_ const &operand () const { return m_operand; }

private:

    void operator = (IndexEmbedder_t const &);

    Operand_ m_operand;
};

// not an expression template, but just something that handles the embedded indices
template <typename Operand_,
          typename SourceAbstractIndexType_,
          typename CoembeddingCodomain_,
          typename CoembeddedAbstractIndexType_,
          typename EmbeddingId_>
struct IndexCoembedder_t
{
    typedef typename AbstractIndicesOfDimIndexTyple_f<typename Operand_::FreeDimIndexTyple>::T OperandFreeAbstractIndexTyple;

    static_assert(IsExpressionTemplate_f<Operand_>::V, "Operand must be an ExpressionTemplate_i");
    static_assert(IsAbstractIndex_f<SourceAbstractIndexType_>::V, "SourceAbstractIndexType_ must be an AbstractIndex_c");
    static_assert(IsAbstractIndex_f<CoembeddedAbstractIndexType_>::V, "CoembeddedAbstractIndexType_ must be an AbstractIndex_c");
    static_assert(Contains_f<OperandFreeAbstractIndexTyple,SourceAbstractIndexType_>::V, "source index must be free");
    static_assert(!TypesAreEqual_f<SourceAbstractIndexType_,CoembeddedAbstractIndexType_>::V, "source and coembedded indices must be distinct");

    typedef typename Operand_::Scalar Scalar;
    // we must replace SourceAbstractIndexType_ with CoembeddedAbstractIndexType_ in the index typle
    static Uint32 const SOURCE_INDEX_TYPE_INDEX = IndexOfFirstOccurrence_f<OperandFreeAbstractIndexTyple,SourceAbstractIndexType_>::V;
    typedef typename Element_f<typename Operand_::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX>::T CoembeddingDomain;

    typedef typename ConcatTyples_f<
        typename LeadingTyple_f<typename Operand_::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX>::T,
        Typle_t<CoembeddingCodomain_>,
        typename TrailingTyple_f<typename Operand_::FreeFactorTyple,SOURCE_INDEX_TYPE_INDEX+1>::T>::T FactorTyple;
    typedef typename ConcatTyples_f<
        typename LeadingTyple_f<typename Operand_::FreeDimIndexTyple,SOURCE_INDEX_TYPE_INDEX>::T,
        Typle_t<CoembeddedAbstractIndexType_>,
        typename TrailingTyple_f<typename Operand_::FreeDimIndexTyple,SOURCE_INDEX_TYPE_INDEX+1>::T>::T AbstractIndexTyple;
    typedef typename DimIndexTypleOf_f<FactorTyple,AbstractIndexTyple>::T DimIndexTyple;
    typedef typename Concat2Typles_f<typename Operand_::UsedDimIndexTyple,Typle_t<CoembeddedAbstractIndexType_>>::T UsedDimIndexTyple;
    typedef MultiIndex_t<DimIndexTyple> MultiIndex;

    static_assert(Length_f<FactorTyple>::V == Length_f<DimIndexTyple>::V, "must have same number of factors and indices");

    IndexCoembedder_t (Operand_ const &operand) : m_operand(operand) { }

    Scalar operator [] (MultiIndex const &m) const
    {
        typedef ComponentIndex_t<DimensionOf_f<CoembeddingCodomain_>::V> CoembeddingCodomainComponentIndex;
        typedef ComponentIndex_t<DimensionOf_f<CoembeddingDomain>::V> CoembeddingDomainComponentIndex;

        CoembeddingCodomainComponentIndex j(m.template el<SOURCE_INDEX_TYPE_INDEX>());

        // the domain and codomain are reversed because of a contravariance property
        typedef typename LinearEmbedding_f<CoembeddingCodomain_,CoembeddingDomain,Scalar,EmbeddingId_,WithExceptions::DISABLED>::T LinearEmbedding;
        typedef typename CoembedIndexIterator_f<CoembeddingCodomain_,CoembeddingDomain,Scalar,EmbeddingId_,WithExceptions::DISABLED>::T CoembedIndexIterator;
        Scalar retval(0);
        for (CoembedIndexIterator it(j); it.is_not_at_end(); ++it)
        {
            static_assert(TypesAreEqual_f<typename CoembedIndexIterator::ComponentIndexReturnType,CoembeddingDomainComponentIndex>::V, "index types must match");
            // this replaces the CoembeddedAbstractIndexType_ portion with SourceAbstractIndexType_
            typename Operand_::MultiIndex c_rebundled(m.template leading_tuple<SOURCE_INDEX_TYPE_INDEX>()
                                                      |
                                                      (it.component_index() >>= m.template trailing_tuple<SOURCE_INDEX_TYPE_INDEX+1>()));
            retval += it.scale_factor() * m_operand[c_rebundled];
        }
        return retval;
    }

    bool overlaps_memory_range (Uint8 const *ptr, Uint32 range) const
    {
        return m_operand.overlaps_memory_range(ptr, range);
    }

    Operand_ const &operand () const { return m_operand; }

private:

    void operator = (IndexCoembedder_t const &);

    Operand_ m_operand;
};

} // end of namespace Tenh

#endif // TENH_EXPRESSION_TEMPLATES_UTILITY_HPP_
