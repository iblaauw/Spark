#include "AST/Comment.h"

RULE(MultiComment);
RULE(LineComment);


RULE(Comment)
{
    Autoname(builder);
    builder.Add(MultiComment);
    builder.Add(LineComment);
}

// Line Comment

static CHARSET(LineCommentChar)
{
    return c != '\n';
}

static RULE(LineCommentContent)
{
    Autoname(builder);
    builder.AddString(LineCommentChar);
    builder.AddEmpty();
}

RULE(LineComment)
{
    Autoname(builder);
    builder.Add("//", LineCommentContent, '\n');
}

// Multi Comment

static CHARSET(MultiCommentChar)
{
    return c != '*';
}


static CHARSET(NonSlash)
{
    return c != '/';
}

static RULE(MultiCommentContent)
{
    Autoname(builder);
    builder.AddString(MultiCommentChar);
    builder.AddEmpty();
}

static RULE(MultiCommentTail)
{
    Autoname(builder);
    builder.Add(MultiCommentContent, "*/");
    builder.Add(MultiCommentContent, "*", NonSlash, MultiCommentTail);
}

RULE(MultiComment)
{
    Autoname(builder);
    builder.Add("/*", MultiCommentTail);
}


