//�ؼ���
#define IF 1	
#define ELSE 2
#define ELSEIF 3
#define FOR 4
#define WHILE 5
#define BREAK 6 
#define CONTINUE 7
#define RETURN 8
#define MAIN 9
#define INTEGER_TYPE 10
#define CHAR_TYPE 11
#define VOID_TYPE 12
#define INCLUDE 13
#define PRINTF 14
//��ʶ��
#define ID 15
//������ �ַ�������
#define INT 16
#define STRING 17
#define CHAR 18
//�����
//���������
#define ADD 19 // +
#define SUB 20 // -
#define MULTI 21 // *
#define DIV 22 // /
#define MOD 23 // %
#define	ADDD 24 // ++
#define SUBB 25 // --
//��ϵ�����
#define SMALL 26 // >
#define BIG 27 // <
#define NSMALL 28 // >=
#define NBIG 29 // <=
#define EQUAL 30 // ==
#define NEQUAL 31 // !=
//�߼������
#define AND 33 // &&
#define OR 34 // ||
#define NOT 35 // !
//��ֵ
#define	ASSIGN  36// =
//�ֽ��
#define LS 37 // (
#define RS 38 // )
#define LB 39 // {
#define RB 40 // }
#define LM 41 // [
#define RM 42 // ]
#define COMMA 43 // ,
#define COLON 44 // :
#define SEMIC 45 // ;
#define LNOTE 46// /*
#define RNOTE 47// */
#define LINENOTE 48// //
#define QUO 49 // "
#define SQUO 50 // '
#define OIL 51 //#
/*
�ؼ���
��ʶ��
����
�����
�ֽ��
*/
class errorl {
public:int hang;
	int type;
	errorl(int hang, int type)
	{
		this->hang = hang;
		this->type = type;  // 0:�Ƿ��ַ� 1:'' 2:"" 3,4:() 5,6:[] 7,8:{} 9:/* */
	}
};
