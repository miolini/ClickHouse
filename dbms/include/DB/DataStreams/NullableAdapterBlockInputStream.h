#pragma once

#include <DB/DataStreams/IProfilingBlockInputStream.h>

namespace DB
{

/// This stream allows perfoming INSERT requests in which the types of
/// the target and source blocks are compatible up to nullability:
///
/// - if a target column is nullable while the corresponding source
/// column is not, we embed the source column into a nullable column;
/// - if a source column is nullable while the corresponding target
/// column is not, we extract the nested column from the source;
/// - otherwise we just perform an identity mapping.
class NullableAdapterBlockInputStream : public IProfilingBlockInputStream
{
public:
	NullableAdapterBlockInputStream(BlockInputStreamPtr input_, const Block & in_sample_,
		const Block & out_sample_, const NamesAndTypesListPtr & required_columns_);

	String getName() const override { return "NullableAdapterBlockInputStream"; }

	String getID() const override;

protected:
	Block readImpl() override;

private:
	/// Given a column of a block we have just read,
	/// how must we process it?
	enum Action
	{
		/// Do nothing.
		NONE = 0,
		/// Convert nullable column to ordinary column.
		TO_ORDINARY,
		/// Convert non-nullable column to nullable column.
		TO_NULLABLE
	};

	/// Actions to be taken for each column of a block.
	using Actions = std::vector<Action>;

private:
	/// Return true if we must transform the blocks we read.
	bool mustTransform() const;

	/// Determine the actions to be taken using the source sample block,
	/// which describes the columns from which we fetch data inside an INSERT
	/// query, and the target sample block which contains the columns
	/// we insert data into.
	Actions getActions(const Block & in_sample, const Block & out_sample) const;

private:
	NamesAndTypesListPtr required_columns;
	const Actions actions;
	const bool must_transform;
};

}
