#include <htxx/html.h>
#include <htxx/html_formatter.h>
#include <htxx/css.h>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <print>
#include "post.h"
#include "template_post.h"

static constexpr auto body_style() {
    using namespace css;
    return style{ $max_width = "80ch",
                  $margin = "1em auto",
                  $padding = "0.62em",
                  $text_align = "center"
    };
}

static constexpr auto post_item_style() {
    using namespace css;
    return style { $border_bottom = "1pt solid lightgray",
                   $padding_top = "2pt",
                   $padding_bottom = "2pt",
                   $height = "80pt",
                   $overflow = "hidden",
                   $text_overflow = "ellipses",
                   $cursor = "pointer"
    };
}

static constexpr auto post_subhed_style() {
    using namespace css;
    return style{ $color = "gray",
                  $overflow = "auto hidden",
                  $max_height = "inherit",
                  $line_clamp = "3",
                  $display = "webkit-box"
    };
}

static constexpr auto index_entry(const post& p) {
    using namespace ml;
    using namespace css;
    using namespace std::string_view_literals;
 
    return a{ $style(css::$color = "inherit", $text_decoration = "none"), $href = p.encoded_name,
            ml::div{ $id = p.encoded_name, $style = post_item_style(),
                span{ $style($font_size = "1.25em", $font_weight = "bold"), p.hed },
                span{ $style(css::$color = "gray", $white_space = "nowrap"), p.date },
                ml::div{ $style = post_subhed_style(), p.subhed }
        }
    };
}

template <class = void> constexpr auto post_list() {
    using namespace ml;
    using namespace css;
    using namespace std::string_view_literals;

    const auto& [...posts] = site_posts;

    return ml::div { $style($text_align = "left", $padding = "1em"),
        ml::div{ $style($padding = "2pt", $border_bottom = "1pt solid lightgray", css::$height = "1pt"), ""sv },
        index_entry(posts)...
    };
}

static constexpr auto index_html() {
    using namespace ml;
    using namespace std::string_view_literals;

    return html{ $lang = "en-US",
        head{
            title{ "kioshi morosin"sv },
            meta{ $name = "viewport", $content = "width=device-width,initial-scale=1", ""sv }
        },
        body{ $style = body_style(),
            span{ "kioshi morosin"sv },
            post_list()
        }
    };
}

static void create_post_dir(const post& p, const std::filesystem::path& path) {
    std::filesystem::create_directories(path);
    auto tex_file = path/p.encoded_name;
    tex_file.replace_extension("tex");
    
    std::ofstream of{ tex_file };
    std::ostream_iterator<char> it{ of };
    
    if (of.is_open()) {
        std::format_to(it, R"(
{}
\title{{{}}}
\date{{{}}}
\begin{{document}}


\end{{document}}

)", post_preamble, p.hed, p.date);
    }    
}

template <class = void> constexpr void create_makefile() {
    const std::filesystem::path makefile_path{ std::filesystem::current_path()/"Makefile" };
    const auto& [...posts] = site_posts;
    std::filesystem::remove(makefile_path);
    std::ofstream ofs{ makefile_path };

    if (!ofs.is_open()) { std::println(stderr, "error opening Makefile"); return; }
    std::ostream_iterator<char> it{ ofs };

    std::format_to(it, R"(
.PHONY: all
all: )");
    (std::format_to(it, "{}/index.html ", posts.encoded_name), ...);

    (std::format_to(it, R"(

{0}/index.html: {0}/{0}.tex
	cd {0}; \
	cp *.bib ../build/; \
	cp *.png ../build/; \
	cd ../build/; \
	ebb -x *.png; \
	make4ht -s -c ../htstyle.cfg -d ../{0}/ ../{0}/{0}.tex "fn-in,mathml"; \
	mv ../{0}/{0}.html ../{0}/index.html;

)", posts.encoded_name), ...); 
}

int main(int argc, char* argv[]) {
    auto workdir = std::filesystem::current_path();
    
    for (const post& p : std::span(site_posts)) {
        auto post_dir = workdir/p.encoded_name;
        if (!std::filesystem::exists(post_dir)) {
            create_post_dir(p, post_dir);
        }
    }

    create_makefile();
    
    std::println("{}", index_html());
    return 0;
}

