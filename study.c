#include <stdio.h>
#include <stdlib.h>

typedef struct Person {	
	char name[256];
	char* apelido;
} person;

void IPN(person *p) {
	printf("Insira um nome: ");
	scanf(" %s", p->name);
}

void IPA(person *p) {
	printf("Insira o apelido: ");
	scanf(" %s", p->apelido);
}

void PP(person *p) {
	printf("Person {nome: %s apelido: %s}\n", p->name, p->apelido);
}

int main() {

	person p;
	IPN(&p);
	IPA(&p);
	PP(&p);

	return 0;
}

/*
 * Um array aloca memória no local onde ele foi declarado. 
 * Então name ocupa 256 bytes dentro da estrutura Person. 
 * Qualquer objeto do tipo person ocupará pouco mais que isto quando for alocado.

Um ponteiro é apenas um apontador para um endereço de memória. 
Ele ocupa apenas 4 bytes (em arquiteturas 32 bits) ou 8 bytes (64 bits). 
É só isso que vai ocupar na estrutura. Ótimo, né? Mas onde o apelido mesmo vai alocado? 
Nesse código vai "em qualquer lugar" que o código achar.

Uma pequena mudança resolverá o problema. 
Basta alocar a memória dinamicamente, 
aí terá um local apropriado para a string ficar na memória:



void insere_person_apelido(person *p) {
    p->apelido = malloc(256);
    printf("insira um apelido: ");
    scanf(" %s", p->apelido);
}



Normalmente isto teria que ser feito de um jeito um pouco melhor. 
Teria que liberar a memória quando p não sirva mais. 
Mas em um exemplo simples, isso não causa problemas.

Um ponteiro tem sempre que apontar para um endereço de memória. 
Pode ser para algo alocado estaticamente pelo código ou dinamicamente pelo malloc(). 
Usei o segundo, mas neste exemplo simples poderia ter usado um outro array alocado na stack e apontar para ele.

Em C é muito fácil pegar lixo da memória se não tomar cuidado.

Lembre-se que o tamanho dos objetos precisam ser conhecidos em tempo de execução antes de alocar. 
Alguns precisam ser conhecidos já em tempo de compilação. Não existe mágica.

Também é importante notar que a notação de acesso à arrays em alguns casos se confunde com ponteiros, mas que fique claro que arrays não são ponteiros.

O exemplo original da pergunta funcionou? Sim. 
Mas tá completamente errado. 
Por isso eu falo o mesmo que profissionais experientes e os novatos não costumam ouvir e por isso a maioria jamais se tornam experientes, mesmo com décadas programando: 
funcionar é circunstancial, 
entender o que está fazendo e produzir um código correto é a única forma correta de programar.

Parecer que funciona não basta. Tem que estar certo.

Por isso "boas práticas" são ruins, 
acreditar que "testar" é a solução é algo ruim também, 
achar que metodologias sozinhas salvam, e olhar só a superfície das coisas não leva alguém à algum lugar.

Esta pergunta é um passo na direção certa para entender o real funcionando, 
que em C é ainda mais importante que outras linguagens, 
já que ela te deixa fazer "tudo" e é problema seu saber se está correto.
*/

/*
Quando você declara um array de char assim:

char name[256];
o programa aloca 256 caracteres na memória principal. 
Quando você declara um ponteiro de char assim:

char *apelido;

o programa aloca o suficiente para guardar um endereço 
(normalmente 4 ou 8 bytes) na memória principal.

Quando você usa name no scanf, ele é tratado como apontador
e a string lida da entrada padrão é armazenada na memória que foi alocada para name. 
Quando você usa apelido no scanf, ele também é tratado como apontador e a string lida da entrada padrão é armazenada na região de memória para a qual apelido aponta. 
Se apelido aponta para uma região de memória que ainda não foi alocada quando scanf tenta escrever lá, 
o programa aborta. 
Se apelido aponta para uma região de memória que foi alocada adequadamente (por exemplo com malloc) 
o programa funciona normalmente.

Você pode mudar a função insere_person_apelido para o seguinte:

void insere_person_apelido(person *p){
    printf("insira um apelido: ");
    p->apelido = malloc(256);
    scanf(" %s", p->apelido);
}

Com o código acima, toda vez que a função executar, 
memória o suficiente para guardar 256 caracteres será alocada 
e o apontador apelido apontará para essa região de memória, 
em que o scanf poderá deixar os caracteres lidos da entrada padrão. 
Se apelido já apontava para uma região de memória alocada anteriormente, 
essa antiga região de memória nunca mais será liberada e ocorrerá vazamento de memória, 
então cuidado com malloc. Outra solução seria você criar uma função que inicializa uma estrutura person, assim:

void inicializar_person(person *p){
    p->apelido = malloc(256);
}

Essa função aloca espaço o suficiente para 256 caracteres e 
faz com que o membro apelido de p aponte para essa nova região de memória. 
Você tem que se lembrar de sempre chamar essa função quando você instanciar uma person. Exemplo:

person p;
inicializar_person(&p);

Assim, toda instância da estrutura person começará em um estado que as outras funções que lidam com person poderão presumir, 
e a função insere_person_apelido poderá ser escrita assim:

void insere_person_apelido(person *p){
    printf("insira um apelido: ");
    scanf(" %s", p->apelido);
}

A função acima presume que apelido aponta para uma região de memória devidamente alocada, 
que sempre é o caso se p foi incializado pela função inicializar_person. 
Se no futuro você acrescentar outro apontador na estrutura person, 
você poderá adequar a função inicializar_person e assim as outras funções poderam presumir que o novo apontador pode ser utilizado. 
Contanto que você apenas utilize instâncias da estrutura person através de funções adequadas em vez de alterar uma person diretamente, 
o seu programa vai funcionar corretamente.

Quando você declara uma variável em C sem inicializá-la, o seu valor inicial pode ser qualquer valor. Exemplo:

int x = 5; // variável inicializada
int y; // variável não inicializada
printf("x = %d\ny = %d", x, y);

Um possível exemplo de saída desse código é:

x = 5
y = 19765

Ou seja, 
como y não foi inicializado, 
o seu valor inicial será qualquer lixo que estava na posição de memória em que o y foi alocado quando o seu programa rodou.

Logo, quando você tenta escrever na memória para onde um apontador não-inicializado aponta, 
você muito provavelmente vai acessar memória que seu programa ainda não alocouadequadamente. 
No entanto, toda string escrita literalmente no código fonte (como "Fulano" ou "Lano" no seu exemplo) é armazenada no arquivo executável do seu programa, 
e quando o programa roda, essa string é armazenada em uma certa região na memória principal, 
logo quando você atribui "Lano" a apelido, esse endereço é guardado em apelido e o programa funciona normalmente. 
O problema com apelido é quando você tenta acessar a memória para onde ele aponta e ele não aponta para uma região de memória válida, 
que nunca é o caso quando ele aponta para uma string que foi escrita literalmente no código fonte.

Acréscimo:

A partir do padrão C99 (pense em C99 como a "versão" da linguagem C que foi publicada em 1999) é possível declarar um array com tamanho variável sem recorrer a malloc, assim:

#include <stdio.h>

int main()
{
    int tamanho;
    printf("Digite um tamanho: ");
    scanf("%d", &tamanho);
    char name[tamanho];
    name[tamanho - 1] = 'x';
    printf("%c", name[tamanho - 1]);
    return 0;
}

Esse código produz a mesma saída que o seguinte:

#include <stdio.h>

int main()
{
    int tamanho;
    printf("Digite um tamanho: ");
    scanf("%d", &tamanho);
    char *name = malloc(tamanho);
    name[tamanho - 1] = 'x';
    printf("%c", name[tamanho - 1]);
    free(name);
    return 0;
}

Antes do padrão C99, não era possível declarar um array com um tamanho conhecido apenas em tempo de execução da forma tradicional, 
então era necessário declarar um ponteiro normal e usar a função malloc para alocar o array. 
No entanto, a partir do C99, é possível fazer isso, 
mas não em estruturas. No seu caso, você tem uma estrutura que precisa guardarum apelido, e como nem todo apelido ocupa o mesmo tamanho,
sugiro que você opte por usar um ponteiro normal e chamar malloc para alocar esse array. 
Se você aceitar que nenhum dos apelidos em seu programa ocupará mais do que sei lá, 
20 caracteres, você pode declarar o membro apelido assim:

char apelido[21];

pois assim o seu código sempre gasta 21 bytes para cada apelido, 
mas como nenhum apelido tem mais do que 20 caracteres,
seu programa não falhará. O problema é garantir isso (se você puder, beleza),
e tem também o fato de que um apelido curto, de por exemplo 10 caracteres, 
desperdiçará espaço (50% nesse caso), então eu sugiro declarar apelido assim (e alocá-lo com malloc):

char *apelido;

Meu conselho é esse: se você não sabe quanto espaço você vai precisar para todos os apelidos (ou qualquer que seja o membro da sua estrutura), 
utilize um ponteiro comum e aloque o array com malloc. 
Se todos os apelidos (ou qualquer que seja o membro da sua estrutura) for ocupar o mesmo tamanho, ou se você aceitar um possível desperdício de espaço, 
declare o array da forma tradicional, com um tamanho fixo. 

Agora, se você quer que todas as instâncias da sua estrutura apontem para o mesmo apelido, 
sem duplicar o conteúdo do apelido em todas as instâncias da estrutura,
também sugiro que você utilize um ponteiro simples e aloque o array com malloc. 
Se você quer um array cujo tamanho só será conhecido em tempo de execução e você precisa desse array diretamente dentro de uma função (e não dentro de uma estrutura como no código que você forneceu) 
e você tem um compilador que suporte C99, então pode simplesmente declarar o array assim:

tipo nome[tamanho];

onde tamanho pode ser uma constante ou o valor de uma variável.
*/
