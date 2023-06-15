from CToJson import  CToJSON
if __name__ == "__main__":
    objCToJSON = CToJSON()
    fileName = "hello.c"
    ast_dict = objCToJSON.file_to_dict(fileName)
    ast = objCToJSON.from_dict(ast_dict)
    print(objCToJSON.to_json(ast,sort_keys=True,indent=2))
    