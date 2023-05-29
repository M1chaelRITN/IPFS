#include "Chunk.h"

const size_t Chunk::SIZE = 262144;

Chunk Chunk::Load(const Value & jsonChunk)
{
	uint64_t fileId = jsonChunk["file_id"].GetUint64();
	uint32_t id = jsonChunk["id"].GetUint();
	bool compressed = jsonChunk["compressed"].GetBool();
	bool local = jsonChunk["local"].GetBool();

	return { FileId(fileId), id, compressed, local };
}

void Chunk::Save(PrettyWriter<StringBuffer> & writer)
{
	writer.StartObject();

	writer.String("file_id");
	writer.Uint64(fileId_.Get());

	writer.String("id");
	writer.Uint(id_);

	writer.String("compressed");
	writer.Bool(compressed_);

	writer.String("local");
	writer.Bool(local_);

	writer.EndObject();
}
