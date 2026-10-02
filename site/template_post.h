#ifndef TEMPLATE_POST_H
#define TEMPLATE_POST_H

#include <meta>
inline constexpr auto post_preamble = std::define_static_string(R"(\documentclass[11pt]{article}

\usepackage{changepage}
\usepackage{amsmath}
\usepackage{amsfonts}
\usepackage{amssymb}
\usepackage{url}
\usepackage{indentfirst}
\usepackage{hanging}
\usepackage{titlesec}
\usepackage{hyperref}

\setlength{\parindent}{0pt}
\setlength{\parskip}{0.5\baselineskip}

\renewcommand{\labelitemi}{\(\hookrightarrow\)}
\setlength{\itemsep}{0pt}
\titleformat{\section}{\bfseries\Large}{\thesection}{1em}{}
\titleformat{\subsection}{\bfseries\large}{\thesubsection}{1em}{}
)");

#endif

