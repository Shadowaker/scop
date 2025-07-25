#ifndef Parser_HPP
# define Parser_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>
# include <array>
# include <memory>

// Forward declaration of the struct
typedef struct s_VertexData {
    float position[3];
    float normal[3];
    float texCoord[2];
} t_VertexData;  


// Class to parse .obj files
class Parser {
    private:

        std::vector<t_VertexData>                   vertices;
        std::vector<std::array<float, 3>>           normals;
        std::vector<std::array<float, 2>>           textures;
		std::vector<std::vector<unsigned int>>		rawFaces;
        std::string                                 filepath;
        
        void                        parseLine(const std::string& line);
        std::vector<std::string>    split(const std::string& str, char delimiter) const;
        float                       stf(const std::string& str) const;
        unsigned int                stui(const std::string& str) const;


    public:
        Parser();
        Parser(const std::string& filepath);
        ~Parser();

        Parser& operator=(const Parser&);

        bool                                                parse(const std::string& filepath);
        const std::vector<t_VertexData>&                    getVertices() const;
        const std::vector<std::array<float, 3>>&            getNormals() const;
        const std::vector<std::array<float, 2>>&            getTextures() const;
        const std::vector<std::vector<unsigned int>>&		getRawFaces() const;

        void                                                printVertices() const;
        void                                                inspect() const;

        
	class ParserException : public std::exception {
		protected:
			std::string _reason;

		public:
			ParserException(std::string reason) : _reason(reason) {};
			virtual ~ParserException() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

    // Default ParseError
    class ParseError : public ParserException{
		protected:
			std::string _reason;

		public:
			ParseError(std::string reason) : ParserException(reason) {};
			virtual ~ParseError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};


    class FileNotFoundError : public ParserException {

		public:
			FileNotFoundError() : ParserException("File not found.") {};
			virtual ~FileNotFoundError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};
};

#endif // Parser_HPP
