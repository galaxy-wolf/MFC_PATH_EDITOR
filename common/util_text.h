#pragma once

namespace util
{

bool IsWhiteSpace(int c);
int GetLine(char *buffer, int maxLen, void *file, bool allowLineContinuations = false);
int GetNonCommentLine(char *buffer, int maxLen, void *file, bool allowLineContinuations = false);

} // namespace util
