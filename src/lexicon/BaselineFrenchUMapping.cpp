#include "BaselineFrenchUMapping.h"

#include <vector>

namespace {
    bool ends_with(const std::string& word, const std::string& suffix)
    {
        //    std::cout << "is " << word << " ending with " << suffix << " ? " ;
        bool result =
            (word.length() >= suffix.length()) &&
            (suffix == word.substr(word.length() - suffix.length(), suffix.length()));

        //    std::cout << (result ? "True" : "False") << std::endl;
        return result;
    }
}

#define ONEMASK ((size_t)(-1) / 0xFF)

static size_t utf8_length(const char * _s)
{
    const char * s;
    size_t count = 0;
    size_t u;
    unsigned char b;

    /* Handle any initial misaligned bytes. */
    for (s = _s; (uintptr_t)(s) & (sizeof(size_t) - 1); s++) {
        b = *s;

        /* Exit if we hit a zero byte. */
        if (b == '\0')
            goto done;

        /* Is this byte NOT the first byte of a character? */
        count += (b >> 7) & ((~b) >> 6);
    }

    /* Handle complete blocks. */
    for (; ; s += sizeof(size_t)) {
        /* Prefetch 256 bytes ahead. */
        __builtin_prefetch(&s[256], 0, 0);

        /* Grab 4 or 8 bytes of UTF-8 data. */
        u = *(size_t *)(s);

        /* Exit the loop if there are any zero bytes. */
        if ((u - ONEMASK) & (~u) & (ONEMASK * 0x80))
            break;

        /* Count bytes which are NOT the first byte of a character. */
        u = ((u & (ONEMASK * 0x80)) >> 7) & ((~u) >> 6);
        count += (u * ONEMASK) >> ((sizeof(size_t) - 1) * 8);
    }

    /* Take care of any left-over bytes. */
    for (; ; s++) {
        b = *s;

        /* Exit if we hit a zero byte. */
        if (b == '\0')
            break;

        /* Is this byte NOT the first byte of a character? */
        count += (b >> 7) & ((~b) >> 6);
    }

done:
    return ((s - _s) - count);
}

std::string BaselineFrenchUMapping::get_unknown_mapping(const std::string& word, unsigned position) const
{
    std::string word_class("UNK");
    std::string word_lower_case=to_lower_case(word);

    bool hasDigit = word.find_first_of("0123456789") != std::string::npos;
    bool hasDash =  word.find_first_of("-") != std::string::npos;
    bool hasLowerCase = false;
    int upperCaseCount=0;

    //std::cout << "this string contains a digit, see " << word << std::endl;
    //std::cout << "this string contains a dash, see " << word << std::endl;

    for(std::string::const_iterator c = word.begin(); c != word.end(); c++){

        if (is_upper_case_letter(*c)){
            //Berkeley parser puts hasLower = true here - an error?
            ++upperCaseCount;
        }else if  (is_lower_case_letter(*c)){
            hasLowerCase = true;
        }
    }

    //	if (upperCaseCount > 0){
    //	std::cout << "original word is " << word << " lower case version is " << word_lower_case << std::endl;
    //}
    //if first character is upper case and it is the first word in sentences and the remaining characters are lower case

    SymbolTable& sym_tab_word = SymbolTable::instance_word();

    //deal with capitalisation
    if ( is_upper_case_letter(*(word.begin()))){
        if (position==0 && upperCaseCount==1){
            word_class.append("-INITC");
            if (sym_tab_word.token_exists(word_lower_case)){
                word_class.append("-KNOWNLC");
            }
        }else {
            word_class.append("-CAPS");
        }
        //missing an else if here - basically, I'm not dealing with non-letters
    }else if (hasLowerCase){
        word_class.append("-LC");
    }

    if (hasDigit){
        word_class.append("-NUM");
    }
    if (hasDash){
        word_class.append("-DASH");
    }

    unsigned word_length = utf8_length( word_lower_case.c_str() );

    if ( (word_length >=3) && (ends_with(word_lower_case,"s") && !ends_with(word_lower_case, "ss")) ){
        word_class.append("-s");
    }

    else

        if (word_length >= 5 && !hasDash && !(hasDigit && upperCaseCount > 0)) {
            // don't do for very short words;
            // Implement common discriminating suffixes

            static std::string sufs[] = { "âtre" , "aphe", "aphie", "ment", "aire", "if",
                "ien", "age", "al", "ale", "ère", "ique",
                "tion", "able", "aux", "enne", "ive", "al",
                "eur", "ois", "oise", "eux"};


            static std::string V_sufs[] = { "issons", "issez", "issent", "isse", "isses", "issions",
                "issiez", /*"issant",*/ "issais", "issait", "issaient", "îmes",
                "îtes", "irent", "irai", "iras", "irons", "iront",
                "irez", "irais", "irait", "irions", "iriez", "iraient",
                "erai", "eras",  "erons", "erez", "eront", "erais",
                "erait", "erions", "eriez", "eraient", "ions", "iez",
                "ant", "ais", "ait", "aient", "as", "âmes",
                "âtes", "èrent", "ons", "ez", "ent", "es",
                "ées", "er", "ir", "oir", "dre"};

            static std::vector<std::string> suffixes(sufs,sufs+22);
            static std::vector<std::string> V_suffixes(V_sufs,V_sufs+52);


            // static std::string ig_sufs[] = { //"ner", "cé", "né", "core", "ont", "ire",
            // 				       //"ble", "lle", "ance", "tent", "voir", "oir",
            // 				       //"ront", "nce", "ce", "ui", "ssi", "ussi",
            // 				       //"uer", "sant", "tait", "dre",
            // 				       "lles", "u'",
            // 				       "elle", "rs", "ndre", "is", "'",
            // 				       "l", "-", "sé", "tant", "ns", "vait",
            // 				       "ées", "res", "ra", "ts", "té", "rer",
            // 				       "ions", "ons", "les", "re", "és", "ser",
            // 				       "si", "a", "ue", "i", "rait", "que",
            // 				       "e", "_que", "le", "ter", "ée", "ir",
            // 				       "n", "es", "ion",
            // 				       "it", "ait", "s", "é",
            // 				       "er", "nt", "r", "t"};


            // static std::vector<std::string> suffixes(ig_sufs,ig_sufs+48);


            bool found = false;
            for(std::vector<std::string>::const_iterator i(suffixes.begin()); i != suffixes.end(); ++i) {
                //	std::cout << "testing " << word_lower_case << " against " << *i << std::endl;
                if(ends_with(word_lower_case, *i)) {
                    //	  std::cout << "testing " << word_lower_case << " against " << *i << std::endl;
                    word_class.append("-"); word_class.append(*i);
                    found = true;
                    break;
                }
            }

            if(!found) {

                for(std::vector<std::string>::const_iterator i(V_suffixes.begin()); i != V_suffixes.end(); ++i) {
                    if(ends_with(word_lower_case, *i)) {
                        word_class.append("-V"); word_class.append(*i);
                        found = true;
                        break;
                    }
                }
            }


        }


    //std::cout << "word class " << word_class << std::endl;

    //    std::cout << word << " " << position << ": " << word_class << std::endl;


    return word_class;

}
