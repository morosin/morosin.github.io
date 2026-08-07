#ifndef POST_H
#define POST_H

#include <algorithm>
#include <chrono>
#include <meta>
#include <string_view>

struct post {
    std::string_view hed;
    std::chrono::year_month_day date;
    std::string_view subhed;
    std::string_view encoded_name;

    consteval post(std::string_view _hed, std::chrono::year_month_day _date, std::string_view _subhed): hed{ _hed }, date{ _date }, subhed{ _subhed } {

        std::string temp;
        temp.reserve(_hed.size());
        std::ranges::replace_copy_if(hed, std::back_inserter(temp), [](char c){ 
            return (c >= 32 && c <= 47) || (c >= 58 && c <= 64) || (c >= 91 && c <= 96) || (c >= 123 && c <= 127);
        }, '-');
        encoded_name = std::define_static_string(temp);
    }
};

inline constexpr post site_posts[]{
    { "web design in c++26", std::chrono::August/1/2026, R"(or, <c style="font:10pt monospace">/usr/include/c++/16/span: In instantiation of ‘constexpr auto&amp; element&lt;El&lt;A ...&gt; &gt;::child(this auto:41&amp;) [with long unsigned int I = 0; auto:41 = const ml::meta&lt;aspect&lt;std::basic_string_view&lt;char, std::char_traits&lt;char&gt; &gt;, aspect_generator&lt;^^std::string_view, static_string&lt;4, char&gt;{std::array&lt;char, 4&gt;{&quot;nam&quot;}}, &lt;lambda closure object&gt;&lt;lambda()&gt;()&gt; &gt;, aspect&lt;std::basic_string_view&lt;char, std::char_traits&lt;char&gt; &gt;, aspect_generator&lt;^^std::string_view, static_string&lt;7, char&gt;{std::array&lt;char, 7&gt;{&quot;conten&quot;}}, &lt;lambda closure object&gt;&lt;lambda()&gt;()&gt; &gt;, const char*&gt;; El = ml::meta; A = {aspect&lt;std::basic_string_view&lt;char, std::char_traits&lt;char&gt; &gt;, aspect_generator&lt;^^std::string_view, static_string&lt;4, char&gt;{std::array&lt;char, 4&gt;{&quot;nam&quot;}}, &lt;lambda closure object&gt;&lt;lambda()&gt;()&gt; &gt;, aspect&lt;std::basic_string_view&lt;char, std::char_traits&lt;char&gt; &gt;, aspect_generator&lt;^^std::string_view, static_string&lt;7, char&gt;{std::array&lt;char, 7&gt;{&quot;conten&quot;}}, &lt;lambda closure object&gt;&lt;lambda()&gt;()&gt; &gt;, const char*}]’:</c>)" },
};

#endif

