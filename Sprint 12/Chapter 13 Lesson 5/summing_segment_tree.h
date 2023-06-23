#pragma once
#include <memory>
#include <utility>

struct IndexSegment {
public:
    size_t length() const {
        return right - left;
    }
    bool empty() const {
        return length() == 0;
    }

    bool Contains(IndexSegment other) const {
        return left <= other.left && other.right <= right;
    }

    static bool AreIntersected(IndexSegment lhs, IndexSegment rhs) {
        return !(lhs.right <= rhs.left || rhs.right <= lhs.left);
    }

    size_t left;
    size_t right;
};

template <typename Data, typename BulkOperation>
class SummingSegmentTree {
public:
    SummingSegmentTree(size_t size)
        : root_(Build({0, size})) {
    }

    Data ComputeSum(IndexSegment segment) const {
        return this->TraverseWithQuery(root_, segment, ComputeSumVisitor{});
    }

    void AddBulkOperation(IndexSegment segment, const BulkOperation& operation) {
        this->TraverseWithQuery(root_, segment, AddBulkOperationVisitor{operation});
    }

private:
    struct Node;
    using NodeHolder = std::unique_ptr<Node>;

    struct Node {
        NodeHolder left;
        NodeHolder right;
        IndexSegment segment;
        Data data;
        BulkOperation postponed_bulk_operation;
    };

    static NodeHolder Build(IndexSegment segment) {
        if (segment.empty()) {
            return nullptr;
        } else if (segment.length() == 1) {
            return std::make_unique<Node>(Node{{}, {}, segment, {}, {}});
        } else {
            const size_t middle = segment.left + segment.length() / 2;
            return std::make_unique<Node>(
                Node{Build({segment.left, middle}), Build({middle, segment.right}), segment, {}, {}});
        }
    }

    template <typename Visitor>
    static typename Visitor::ResultType TraverseWithQuery(const NodeHolder& node, IndexSegment query_segment,
                                                          Visitor visitor) {
        if (!node || !IndexSegment::AreIntersected(node->segment, query_segment)) {
            return visitor.ProcessEmpty(node);
        } else {
            PropagateBulkOperation(node);
            if (query_segment.Contains(node->segment)) {
                return visitor.ProcessFull(node);
            } else {
                if constexpr (std::is_void_v<typename Visitor::ResultType>) {
                    TraverseWithQuery(node->left, query_segment, visitor);
                    TraverseWithQuery(node->right, query_segment, visitor);
                    return visitor.ProcessPartial(node, query_segment);
                } else {
                    return visitor.ProcessPartial(node, query_segment,
                                                  TraverseWithQuery(node->left, query_segment, visitor),
                                                  TraverseWithQuery(node->right, query_segment, visitor));
                }
            }
        }
    }

    class ComputeSumVisitor {
    public:
        using ResultType = Data;

        Data ProcessEmpty(const NodeHolder&) const {
            return {};
        }

        Data ProcessFull(const NodeHolder& node) const {
            return node->data;
        }

        Data ProcessPartial(const NodeHolder&, IndexSegment, const Data& left_result, const Data& right_result) const {
            return left_result + right_result;
        }
    };

    class AddBulkOperationVisitor {
    public:
        using ResultType = void;

        explicit AddBulkOperationVisitor(const BulkOperation& operation)
            : operation_(operation) {
        }

        void ProcessEmpty(const NodeHolder&) const {
        }

        void ProcessFull(const NodeHolder& node) const {
            node->postponed_bulk_operation.CombineWith(operation_);
            node->data = operation_.Collapse(node->data, node->segment);
        }

        void ProcessPartial(const NodeHolder& node, IndexSegment) const {
            node->data = (node->left ? node->left->data : Data()) + (node->right ? node->right->data : Data());
        }

    private:
        const BulkOperation& operation_;
    };

    static void PropagateBulkOperation(const NodeHolder& node) {
        for (auto* child_ptr : {node->left.get(), node->right.get()}) {
            if (child_ptr) {
                child_ptr->postponed_bulk_operation.CombineWith(node->postponed_bulk_operation);
                child_ptr->data = node->postponed_bulk_operation.Collapse(child_ptr->data, child_ptr->segment);
            }
        }
        node->postponed_bulk_operation = BulkOperation();
    }

private:
    NodeHolder root_;
};
