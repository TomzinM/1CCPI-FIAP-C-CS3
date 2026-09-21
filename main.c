#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "cJSON.h"
#include "cJSON.c"
#define FILE_NAME "users.json"

// Grande parte do codigo foi aprimorada em relação a ultima vez, especificamente as funções recarga() e carro() que tiverão todas suas redundancias removidas
// Além disso, um sistema de tempo simulado foi adicionado, e a 'bateria' local usada na recarga() foi mudada. Agora, o sistema deve sentir mais como se estivesse carregando
// em comparação com antes que era um simulador bem mal feito. Todas recargas estão logadas, o horario (simulado) de cada um, e é tudo salvo no users.json.


// O struct usado para todas sessões
typedef struct {
    int id;
    char nome[100];
    float energia;
    float tempo;
    float preco;
    int seg;
    int min;
    int hora;
    int dia;
    int mes;
    int ano;

} Sessao;

// Essa função salva o texto da root no JSON, e então deleta a root em si.
void save_json(cJSON *root) {
    char *out = cJSON_Print(root);
    FILE *f = fopen(FILE_NAME, "w");
    fputs(out, f);
    fclose(f);
    cJSON_Delete(root);
}

// Essa função serve para criar os parâmetros basicos dentro do JSON, com o nome, senha, e os 3 carros que ainda estão vazios.
int sign_up(char *user, char *pass) {
    char buf[4096];
    FILE *f = fopen(FILE_NAME, "r");
    int bytes = fread(buf, 1, sizeof(buf) - 1, f);
    buf[bytes] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(buf);
    cJSON *new_user = cJSON_CreateObject();
    cJSON *u;

    cJSON_ArrayForEach(u, root) {
        if (strcmp(cJSON_GetObjectItem(u, "username")->valuestring, user) == 0) {
            printf("\nVoce ja tem uma conta com esse nome.\n\n");
            return 0;
        }
    }
    cJSON_AddStringToObject(new_user, "username", user);
    cJSON_AddStringToObject(new_user, "password", pass);
    cJSON_AddStringToObject(new_user, "carro1", "VAZIO");
    cJSON_AddStringToObject(new_user, "carro2", "VAZIO");
    cJSON_AddStringToObject(new_user, "carro3", "VAZIO");

    cJSON_AddItemToArray(root, new_user);
    
    save_json(root);
    printf("Registered!\n");
    return 1;
}

// Essa função serve para fazer o login, checando sua resposta com a informação posta no JSON no sign_up.
int log_in(char *user, char *pass) {
    char buf[4096];
    FILE *f = fopen(FILE_NAME, "r");
    int bytes = fread(buf, 1, sizeof(buf) - 1, f);
    buf[bytes] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(buf);
    cJSON *u;
    cJSON_ArrayForEach(u, root) {
        if (strcmp(cJSON_GetObjectItem(u, "username")->valuestring, user) == 0 &&
            strcmp(cJSON_GetObjectItem(u, "password")->valuestring, pass) == 0) {
            cJSON_Delete(root);
            return 1; 
        }
    }
    cJSON_Delete(root);
    return 0; 
}

// Função da interface principal, com o horario simulado que é acrescentado em cada recarga.
void interface(char *name, float horasAcumuladas, float bateriaEstacao) {
    time_t rawtime = time(NULL);
    rawtime += (long)(horasAcumuladas * 3600);
    struct tm *timeinfo = localtime(&rawtime);

    printf("------------------------------------------\n");
    printf("Bem vindo, %s.\n", name);
    printf("Horario Atual: %02d:%02d:%02d (%02d/%02d/%04d)\n", 
           timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
           timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
    printf("Carga da bateria extra: %.02f", bateriaEstacao);
    printf("\nEscolha o numero relacionado com as opcoes a seguir para continuar.\n");
    printf("------------------------------------------\n");
}

// Animação de recarga.
void carregando(float tempoHoras) {
    printf("\nIniciando recarga virtual de %.2f horas...\n", tempoHoras);
    printf("[");
    for (int i = 0; i < 20; i++) {
        printf("#");
        fflush(stdout); // Força o C a mostrar o caractere na tela imediatamente
        usleep(80000);  // Pausa de 80 milissegundos por caractere para simular velocidade
    }
    printf("] 100%% CONCLUIDO!\n\n");
}

// Função que cria um log de texto das recargas feitas durante uma sessão, e adiciona a structs da main, que podem ser visualizadas no menu principal.
void purchaseLog(char *nome, float price, int condition, int pagamento, float tempo, float energy, Sessao sessoes[], int *total){
    
    carregando(tempo);
    
    time_t rawtime = time(NULL);
    struct tm *timeinfo = localtime(&rawtime);

    long segundos_adicionais = (long)(tempo * 3600);

    rawtime += segundos_adicionais;

    timeinfo = localtime(&rawtime);

    int year = timeinfo->tm_year + 1900;
    int month = timeinfo->tm_mon + 1;
    int day = timeinfo->tm_mday;
    int hour = timeinfo->tm_hour;   
    int min = timeinfo->tm_min;     
    int sec = timeinfo->tm_sec;
    
    if (*total < 100) {
        int indice = *total;
        sessoes[indice].id = indice + 1;
        strcpy(sessoes[indice].nome, nome);
        sessoes[indice].tempo = tempo;
        sessoes[indice].preco = price;
        sessoes[indice].energia = energy;
        sessoes[indice].dia = day; sessoes[indice].mes = month; sessoes[indice].ano = year;
        sessoes[indice].hora = hour; sessoes[indice].min = min; sessoes[indice].seg = sec;
        (*total)++; 
    }

    FILE *fptr = fopen("log.txt", "a");
    if (fptr == NULL) return;

    if (condition == 1){
        fprintf(fptr, "\nRecarga feita com prioridade Solar feita por %s em %02d/%02d/%d.\n", nome, day, month, year);
        fprintf(fptr, "Duracao simulada: %.2f horas. Concluida as: %02d:%02d:%02d\n", tempo, hour, min, sec);
    }
    if (condition == 2){
        fprintf(fptr, "\nRecarga feita com uso de bateria e energia solar feita por %s em %02d/%02d/%d.\n", nome, day, month, year);
        fprintf(fptr, "Duracao simulada: %.2f horas. Concluida as: %02d:%02d:%02d\n", tempo, hour, min, sec);
    }
    if (condition == 3 && pagamento == 1){
        fprintf(fptr, "\nCompra de recarga de R$%.2f feita por %s usando Pix em %02d/%02d/%d.\n", price, nome, day, month, year);
        fprintf(fptr, "Duracao simulada: %.2f horas. Concluida as: %02d:%02d:%02d\n", tempo, hour, min, sec);
    }
    if (condition == 3 && pagamento == 2){
        fprintf(fptr, "\nCompra de recarga de R$%.2f feita por %s usando Cartão de Credito em %02d/%02d/%d.\n", price, nome, day, month, year);
        fprintf(fptr, "Duracao simulada: %.2f horas. Concluida as: %02d:%02d:%02d\n", tempo, hour, min, sec);
    }

    fclose(fptr);
}
    
// A logica da recarga() extraida.
int processarRecargaVeiculo(char *nomeUsuario, int *bateriaCarro, float capacidadeCarro, 
                            Sessao sessoes[], int *total, float *tempoAcumulado, float *bateriaEstacao, 
                            cJSON *target_user, char *tagRecarga, char *tagBateria, cJSON *root) {
    int ch, modo_recarga;
    int temp_battery = *bateriaCarro;
    float kwh_preco = 1.35;

    printf("Ate qual porcentagem quer carregar?\n");
    scanf("%d", bateriaCarro);
    while ((ch = getchar()) != '\n' && ch != EOF); 

    if (*bateriaCarro > 100 || *bateriaCarro < 0 || *bateriaCarro < temp_battery) {
        printf("\nBateria invalida.\n");
        *bateriaCarro = temp_battery;
        return 0;
    }
    
    float bateria_kwh = (*bateriaCarro - temp_battery) * (capacidadeCarro / 100.0);

    printf("Qual modo de recarga quer usar?\n");
    printf("1. Prioridade Solar\n2. Solar + Bateria\n3. Carregamento Rapido\n4. Cancelar\n");
    scanf("%d", &modo_recarga);
    while ((ch = getchar()) != '\n' && ch != EOF); 
    
    time_t tempo_base = time(NULL);
    tempo_base += (long)(*tempoAcumulado * 3600); 
    struct tm *timeinfo_simulado = localtime(&tempo_base);
    float hourtime = timeinfo_simulado->tm_hour + (timeinfo_simulado->tm_min / 60.0);

    switch(modo_recarga) {
        case 1: { 
            float tempo_gasto = (12.0 * bateria_kwh) / 60.0;
            char resp;
            printf("Com energia solar, ira demorar %.2f horas para recarregar %.2f kWh.\nTudo certo com isso? (S/N) ", tempo_gasto, bateria_kwh);
            scanf(" %c", &resp);
            while ((ch = getchar()) != '\n' && ch != EOF); 

            if (resp == 'S' || resp == 's') {
                purchaseLog(nomeUsuario, 0, 1, 0, tempo_gasto, bateria_kwh, sessoes, total);
                *tempoAcumulado += tempo_gasto;
                
                cJSON_DeleteItemFromObject(target_user, tagBateria);
                cJSON_AddNumberToObject(target_user, tagBateria, *bateriaCarro);
                cJSON_AddNumberToObject(target_user, tagRecarga, 1);
                save_json(root);
                return 1;
            }
            break;
        }
        case 2: {

            float tempo_gasto = (6.0 * bateria_kwh) / 60.0;
            
            float energia_bateria_estacao = bateria_kwh * 0.5; 
            char resp;

            if (energia_bateria_estacao >= *bateriaEstacao){

                printf("\n--- Modo Hibrido Ativado ---\n");
                printf("Os paineis solares cobrirao parte da demanda (%.2f kWh de graca!).\n", bateria_kwh - *bateriaEstacao);
                printf("A bateria local da estacao fornecera os %.2f kWh restantes.\n", *bateriaEstacao);
                printf("Capacidade atual da estacao: %.2f kWh\n", *bateriaEstacao);
                printf("Tempo estimado: %.2f horas.\n", tempo_gasto);
                printf("Tudo certo com isso? (S/N) ");
                scanf(" %c", &resp);
                while ((ch = getchar()) != '\n' && ch != EOF);

            }

            else {

                printf("\n--- Modo Hibrido Ativado ---\n");
                printf("Os paineis solares cobrirao parte da demanda (%.2f kWh de graca!).\n", energia_bateria_estacao);
                printf("A bateria local da estacao fornecera os %.2f kWh restantes.\n", energia_bateria_estacao);
                printf("Capacidade atual da estacao: %.2f kWh\n", *bateriaEstacao);
                printf("Tempo estimado: %.2f horas.\n", tempo_gasto);
                printf("Tudo certo com isso? (S/N) ");
                scanf(" %c", &resp);
                while ((ch = getchar()) != '\n' && ch != EOF);

            }
             

            if (resp == 'S' || resp == 's') {
                if (energia_bateria_estacao <= *bateriaEstacao) {
                    *bateriaEstacao -= energia_bateria_estacao;
                } else {
                    *bateriaEstacao = 0;
                }
                
                purchaseLog(nomeUsuario, 0, 2, 0, tempo_gasto, bateria_kwh, sessoes, total);
                *tempoAcumulado += tempo_gasto;

                cJSON_DeleteItemFromObject(target_user, tagBateria);
                cJSON_AddNumberToObject(target_user, tagBateria, *bateriaCarro);
                cJSON_AddNumberToObject(target_user, tagRecarga, 1);
                save_json(root);
                return 1;
            }
            break;
        }
        case 3: { 
            float preco_recarga = 0;
            char usar_bateria, horario_de_recarga, resp_final;

            printf("\nA bateria da estacao possui %.2f kWh disponiveis.\n", *bateriaEstacao);
            printf("Deseja utilizar a energia da bateria para abater no custo? (S/N): ");
            scanf(" %c", &usar_bateria);
            while ((ch = getchar()) != '\n' && ch != EOF);

            float energia_cidade = bateria_kwh;
            float bateria_usada_estacao = 0;

            if ((usar_bateria == 'S' || usar_bateria == 's') && *bateriaEstacao > 0) {
                if (bateria_kwh <= *bateriaEstacao) {
                    energia_cidade = 0;
                    bateria_usada_estacao = bateria_kwh;
                } else {
                    energia_cidade = bateria_kwh - *bateriaEstacao;
                    bateria_usada_estacao = *bateriaEstacao;
                }
            }
            preco_recarga = kwh_preco * energia_cidade;

            if ((hourtime < 18.5 || hourtime > 22.0) && energia_cidade > 0) {
                printf("O horario simulado atual e %02d:%02d (Fora do promocional 18:30 - 22:00).\n", timeinfo_simulado->tm_hour, timeinfo_simulado->tm_min);
                printf("Voce quer esperar ate o inicio do periodo mais barato (18:30)? (S/N) ");
                scanf(" %c", &horario_de_recarga);
                while ((ch = getchar()) != '\n' && ch != EOF);

                if (horario_de_recarga == 'S' || horario_de_recarga == 's') {
                    float espera = (hourtime < 18.5) ? (18.5 - hourtime) : ((24.0 - hourtime) + 18.5);
                    printf("\nAguardando... Avancando %.2f horas ate as 18:30...\n", espera);
                    usleep(1000000);
                    *tempoAcumulado += espera;
                    hourtime = 18.5; 
                } else {
                    preco_recarga *= 1.5; 
                }                        
            }

            float tempo_gasto = (3.0 * bateria_kwh) / 60.0;
            printf("Com carregamento rapido, ira demorar %.2f horas para recarregar %.2f kWh e custara R$ %.2f.\nTudo certo com isso? (S/N) ", tempo_gasto, bateria_kwh, preco_recarga);
            scanf(" %c", &resp_final);
            while ((ch = getchar()) != '\n' && ch != EOF);

            if (resp_final == 'S' || resp_final == 's') {
                int p_pag = 1;
                if (cJSON_HasObjectItem(target_user, "chavePix") && cJSON_HasObjectItem(target_user, "cartaoEmpresa")) {
                    printf("Quer pagar com o seu pix (1) ou cartao de credito (2)? ");
                    scanf("%d", &p_pag);
                    while ((ch = getchar()) != '\n' && ch != EOF);
                } else if (!cJSON_HasObjectItem(target_user, "chavePix")) {
                    p_pag = 2;
                }

                purchaseLog(nomeUsuario, preco_recarga, 3, p_pag, tempo_gasto, bateria_kwh, sessoes, total);
                
                *bateriaEstacao -= bateria_usada_estacao;
                *tempoAcumulado += tempo_gasto;

                cJSON_DeleteItemFromObject(target_user, tagBateria);
                cJSON_AddNumberToObject(target_user, tagBateria, *bateriaCarro);
                cJSON_AddNumberToObject(target_user, tagRecarga, 1);
                save_json(root);
                return 1;
            }
            break;
        }
    }
    *bateriaCarro = temp_battery; 
    return 0;
}

// Função de recarga de todos os carros
// Tem 4 modos de prioridade, e apenas o modo 'rapido' custa dinheiro, pois puxa energia da cidade invés de paines solares ou uma bateria.
// Alem disso, o tempo de cada recarga é contado e acumulado, e a bateria usada na opções de recarga rapida e solar + bateria também tem sua energia contada, resetando apenas entre execuções do programa.

char recharge(char *name, Sessao sessoes[], int *total, float *tempoAcumulado, float *bateriaEstacao) {
    int ch, resposta, recarga = 1;
    char buf[4096];

    FILE *f = fopen(FILE_NAME, "r");
    if (!f) return 'n';
    int bytes = fread(buf, 1, sizeof(buf) - 1, f);
    buf[bytes] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(buf);
    cJSON *target_user = NULL, *user_element = NULL;
    
    cJSON_ArrayForEach(user_element, root) {
        cJSON *user_obj = cJSON_GetObjectItemCaseSensitive(user_element, "username");
        if (user_obj && user_obj->valuestring && strcmp(user_obj->valuestring, name) == 0) {
            target_user = user_element;
            break;
        }
    }

    char *carro1 = cJSON_GetObjectItemCaseSensitive(target_user, "carro1")->valuestring;
    char *carro2 = cJSON_GetObjectItemCaseSensitive(target_user, "carro2")->valuestring;
    char *carro3 = cJSON_GetObjectItemCaseSensitive(target_user, "carro3")->valuestring;

    if (strcmp(carro1, "VAZIO") == 0 && strcmp(carro2, "VAZIO") == 0 && strcmp(carro3, "VAZIO") == 0) {
        printf("Por favor registre seu carro(s) primeiro.");
        cJSON_Delete(root);
        return 'n';
    }

    int c1_bat = cJSON_HasObjectItem(target_user, "carro1_bateria") ? cJSON_GetObjectItem(target_user, "carro1_bateria")->valueint : 0;
    int c2_bat = cJSON_HasObjectItem(target_user, "carro2_bateria") ? cJSON_GetObjectItem(target_user, "carro2_bateria")->valueint : 0;
    int c3_bat = cJSON_HasObjectItem(target_user, "carro3_bateria") ? cJSON_GetObjectItem(target_user, "carro3_bateria")->valueint : 0;

    while (recarga == 1) {
        printf("\n\n== Recarga de Carro ==\n\n");
        printf("1. %s - %d%%\n2. %s - %d%%\n3. %s - %d%%\n4. Sair\n", carro1, c1_bat, carro2, c2_bat, carro3, c3_bat);
        printf("Escolha: ");
        scanf("%d", &resposta);
        while ((ch = getchar()) != '\n' && ch != EOF); 

        // Validações de re-recarga rápidas
        if (resposta == 1 && cJSON_HasObjectItem(target_user, "carro1_recarga")) {
            char resp; printf("Ja recarregou o carro 1 nesta sessao. Repetir? (S/N) "); scanf(" %c", &resp);
            if (resp == 'S' || resp == 's') cJSON_DeleteItemFromObject(target_user, "carro1_recarga"); else resposta = 0;
        }
        if (resposta == 2 && cJSON_HasObjectItem(target_user, "carro2_recarga")) {
            char resp; printf("Ja recarregou o carro 2 nesta sessao. Repetir? (S/N) "); scanf(" %c", &resp);
            if (resp == 'S' || resp == 's') cJSON_DeleteItemFromObject(target_user, "carro2_recarga"); else resposta = 0;
        }
        if (resposta == 3 && cJSON_HasObjectItem(target_user, "carro3_recarga")) {
            char resp; printf("Ja recarregou o carro 3 nesta sessao. Repetir? (S/N) "); scanf(" %c", &resp);
            if (resp == 'S' || resp == 's') cJSON_DeleteItemFromObject(target_user, "carro3_recarga"); else resposta = 0;
        }

        if ((resposta == 1 && strcmp(carro1, "VAZIO") == 0) || (resposta == 2 && strcmp(carro2, "VAZIO") == 0) || (resposta == 3 && strcmp(carro3, "VAZIO") == 0)) {
            printf("Registre esse carro primeiro para carrega-lo.\n");
            resposta = 0;
        }

        switch(resposta) {
            case 1:
                if (processarRecargaVeiculo(name, &c1_bat, cJSON_GetObjectItem(target_user, "carro1_capacidade")->valuedouble, sessoes, total, tempoAcumulado, bateriaEstacao, target_user, "carro1_recarga", "carro1_bateria", root)) return 's';
                break;
            case 2:
                if (processarRecargaVeiculo(name, &c2_bat, cJSON_GetObjectItem(target_user, "carro2_capacidade")->valuedouble, sessoes, total, tempoAcumulado, bateriaEstacao, target_user, "carro2_recarga", "carro2_bateria", root)) return 's';
                break;
            case 3:
                if (processarRecargaVeiculo(name, &c3_bat, cJSON_GetObjectItem(target_user, "carro3_capacidade")->valuedouble, sessoes, total, tempoAcumulado, bateriaEstacao, target_user, "carro3_recarga", "carro3_bateria", root)) return 's';
                break;
            case 4:
                recarga = 0;
                break;
        }
    }
    cJSON_Delete(root);
    return 0;
}


// Funcão que adiciona informação de pagamento, sem registrar isso não tem como usar a recarga rápida.
void pagamento(char *user, char *pass){
    int pagamento;
    char buf[4096];
    char cpf[13];
    char pass2[100];
    int ch;
    char novoregistro;

    FILE *f = fopen(FILE_NAME, "r");
    int bytes = fread(buf, 1, sizeof(buf) - 1, f);
    buf[bytes] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(buf);

    cJSON *target_user = NULL;
    cJSON *user_element = NULL;
    
    cJSON_ArrayForEach(user_element, root) {
        cJSON *user_obj = cJSON_GetObjectItemCaseSensitive(user_element, "username");
        
        if (user_obj && user_obj->valuestring && strcmp(user_obj->valuestring, user) == 0) {
            target_user = user_element;
            break;
        }
    }

    cJSON_DeleteItemFromObject(target_user, "cpf");

    printf("Qual forma de pagamento quer usar para o sistema GoodWe?\n1 - Pix\n2 - Cartao de Credito\n");
    scanf("%d",&pagamento);
    while ((ch = getchar()) != '\n' && ch != EOF); 


    switch(pagamento){
        case 1:
             if (cJSON_HasObjectItem(target_user, "chavePix")){
                printf("Sua informacao ja esta registrada. Quer registrar novamente? (S/N)\n");
                scanf("%c", &novoregistro);
                while ((ch = getchar()) != '\n' && ch != EOF); 

                if (novoregistro == 'S' || novoregistro == 's'){
                    printf("Escreva sua senha para registrar novamente:\n");
                    fgets(pass2, sizeof(pass2), stdin);
                    pass2[strcspn(pass2, "\n")] = '\0';

                    if (strcmp(pass2, pass) != 0){
                        printf("Senha errada.");
                        break;
                    }
                    else {
                        cJSON_DeleteItemFromObject(target_user, "chavePix");      
                    }
                }
            }
            break;
        case 2:

            if (cJSON_HasObjectItem(target_user, "cartaoEmpresa")){
                printf("Sua informacao ja esta registrada. Quer registrar novamente? (S/N) ");
                scanf("%c", &novoregistro);
                while ((ch = getchar()) != '\n' && ch != EOF); 

                if (novoregistro == 'S' || novoregistro == 's'){
                    printf("Escreva sua senha para registrar novamente: ");
                    fgets(pass2, sizeof(pass2), stdin);
                    pass2[strcspn(pass2, "\n")] = '\0';

                    if (strcmp(pass2, pass) != 0){
                        printf("Senha errada.\n");
                        break;
                    }
                    else {
                        cJSON_DeleteItemFromObject(target_user, "cartaoEmpresa");
                        cJSON_DeleteItemFromObject(target_user, "cartaoNumero");
                        cJSON_DeleteItemFromObject(target_user, "cartaoPin");
                        cJSON_DeleteItemFromObject(target_user, "cartaoData");
                        cJSON_DeleteItemFromObject(target_user, "cartaoCep");              
                    }
                }
            }
            break;
    }
    
    
    printf("Qual seu CPF? \n");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = '\0';
    if (strlen(cpf) != 11){
        printf("CPF Invalido, numero de digitos incorreto.\n");
        cJSON_Delete(root); 
        return;
    }

    cJSON_AddStringToObject(target_user, "cpf", cpf);


    switch(pagamento){
        case 1:
            while ((ch = getchar()) != '\n' && ch != EOF); 
            char chave[100];      

            printf("Qual e sua chave do Pix? ");
            fgets(chave, sizeof(chave), stdin);
            chave[strcspn(chave, "\n")] = '\0';

            cJSON_AddStringToObject(target_user, "chavePix", chave);
            save_json(root);

            break;

        case 2: 
            ;
            char cartaoEmpresa[100];
            char cartaoNumero[18];
            char cartaoPin[6];
            char cartaoData[9];
            char cartaoCep[11];


            printf("Qual empresa e a operadora do seu cartao? ");
            fgets(cartaoEmpresa, sizeof(cartaoEmpresa), stdin);
            cartaoEmpresa[strcspn(cartaoEmpresa, "\n")] = '\0';
            
            printf("Numero da frente do cartao: ");
            fgets(cartaoNumero, sizeof(cartaoNumero), stdin);
            cartaoNumero[strcspn(cartaoNumero, "\n")] = '\0';

            printf("Numero pin do cartao: ");
            fgets(cartaoPin, sizeof(cartaoPin), stdin);
            cartaoPin[strcspn(cartaoPin, "\n")] = '\0';

            printf("Data de expiracao: (MM/YYYY inclua a barra) ");
            fgets(cartaoData, sizeof(cartaoData), stdin);
            cartaoData[strcspn(cartaoData, "\n")] = '\0';

            printf("CEP: (00000-000 inclua o hifen) ");
            fgets(cartaoCep, sizeof(cartaoCep), stdin);
            cartaoCep[strcspn(cartaoCep, "\n")] = '\0';

                cJSON_AddStringToObject(target_user, "cartaoEmpresa", cartaoEmpresa);
                cJSON_AddStringToObject(target_user, "cartaoNumero", cartaoNumero);
                cJSON_AddStringToObject(target_user, "cartaoPin", cartaoPin);
                cJSON_AddStringToObject(target_user, "cartaoData", cartaoData);
                cJSON_AddStringToObject(target_user, "cartaoCep", cartaoCep);
                save_json(root);
            break;

        default:
            printf("Opcao invalida.\n");
            cJSON_Delete(root);
            return;
    }

}

// Logica extraida do carro()
int gerenciarVeiculoIndividual(char *nomeCarro, char *tagNome, char *tagMarca, 
                               char *tagCapacidade, char *tagAutonomia, char *tagBateria, cJSON *target_user, cJSON *root) {
    int ch;
    int carro_resposta = 0;
    int redefinir_sinalizado = 0;

    if (strcmp(nomeCarro, "VAZIO") != 0) {
        cJSON *obj_marca = cJSON_GetObjectItem(target_user, tagMarca);
        cJSON *obj_cap = cJSON_GetObjectItem(target_user, tagCapacidade);
        cJSON *obj_aut = cJSON_GetObjectItem(target_user, tagAutonomia);
        cJSON *obj_bat = cJSON_GetObjectItem(target_user, tagBateria);

        char *marca_json = obj_marca->valuestring;
        float capacidade_json = obj_cap->valueint; 
        float autonomia_json = obj_aut->valueint;
        int bateria_json = obj_bat->valueint;
        float distancia = autonomia_json / capacidade_json;

        while (carro_resposta != 3) {
            printf("\n\n== %s ==\n\n", nomeCarro);
            printf("Carro da marca \"%s\"\n", marca_json);
            printf("Capacidade total: %.2f kWh\n", capacidade_json);
            printf("Eficiencia do carro: %.2f km/kWh\n", distancia);
            printf("Bateria Atual: %d%%\n", bateria_json);

            printf("\n1. Redefinir informacoes\n2. Atualizacao de bateria\n3. Sair\n");
            printf("Escolha: ");
            scanf("%d", &carro_resposta);
            while ((ch = getchar()) != '\n' && ch != EOF);
            
            switch(carro_resposta) {
                case 1:
                    redefinir_sinalizado = 1;
                    carro_resposta = 3; 
                    break;
                case 2: {
                    int temp_bateria = bateria_json;
                    printf("Qual sua nova bateria atual? ");
                    scanf("%d", &bateria_json);
                    while ((ch = getchar()) != '\n' && ch != EOF);

                    if (bateria_json > 100 || bateria_json < 0) {
                        printf("Numero invalido\n");
                        bateria_json = temp_bateria;
                        break;
                    }

                    cJSON_DeleteItemFromObject(target_user, tagBateria);
                    cJSON_AddNumberToObject(target_user, tagBateria, bateria_json);
                    save_json(root);
                    return 0; 
                }
                case 3:
                    return 0; 
                default:
                    break;
            }
        }
    }

    if (strcmp(nomeCarro, "VAZIO") == 0 || redefinir_sinalizado == 1) {
        char novo_nome[100];
        char nova_marca[100];
        float nova_capacidade;
        float nova_autonomia;
        int nova_bateria;

        printf("\nQual o nome do carro que quer registrar? ");
        fgets(novo_nome, sizeof(novo_nome), stdin);
        novo_nome[strcspn(novo_nome, "\n")] = '\0';

        printf("Qual e a marca do carro? ");
        fgets(nova_marca, sizeof(nova_marca), stdin);
        nova_marca[strcspn(nova_marca, "\n")] = '\0';

        printf("Qual a capacidade de energia do carro em kWh? ");
        scanf("%f", &nova_capacidade);
        while ((ch = getchar()) != '\n' && ch != EOF);

        printf("Qual a autonomia total do carro com a carga cheia (em km)? ");
        scanf("%f", &nova_autonomia);
        while ((ch = getchar()) != '\n' && ch != EOF);

        printf("Qual a bateria atual do carro? ");
        scanf("%d", &nova_bateria);
        while ((ch = getchar()) != '\n' && ch != EOF);
               
        if (nova_bateria > 100 || nova_bateria < 0) {
            printf("Numero invalido. Registro cancelado.\n");
            return 0;
        }

        cJSON_DeleteItemFromObject(target_user, tagNome);
        cJSON_DeleteItemFromObject(target_user, tagMarca);
        cJSON_DeleteItemFromObject(target_user, tagCapacidade);
        cJSON_DeleteItemFromObject(target_user, tagAutonomia);
        cJSON_DeleteItemFromObject(target_user, tagBateria);

        cJSON_AddStringToObject(target_user, tagNome, novo_nome);
        cJSON_AddStringToObject(target_user, tagMarca, nova_marca);
        cJSON_AddNumberToObject(target_user, tagCapacidade, nova_capacidade);
        cJSON_AddNumberToObject(target_user, tagAutonomia, nova_autonomia);
        cJSON_AddNumberToObject(target_user, tagBateria, nova_bateria);
        
        save_json(root);
        printf("Carro registrado com sucesso!\n");
    }
    return 0;
}


// Essa função registra os carros, e demonstra a informação deles.
// A parte de atualizar a bateria é para simular o uso do carro e a drenagem natural, para poder testar a função de recarga novamente.
int carro(char *user) {
    char buf[32768];
    int escolha, ch;
    int rodando_menu_carro = 1;

    while (rodando_menu_carro == 1) {
        FILE *f = fopen(FILE_NAME, "r");
        if (!f) return 0;
        int bytes = fread(buf, 1, sizeof(buf) - 1, f);
        buf[bytes] = '\0';
        fclose(f);

        cJSON *root = cJSON_Parse(buf);
        cJSON *target_user = NULL, *user_element = NULL;
        
        cJSON_ArrayForEach(user_element, root) {
            cJSON *user_obj = cJSON_GetObjectItemCaseSensitive(user_element, "username");
            if (user_obj && user_obj->valuestring && strcmp(user_obj->valuestring, user) == 0) {
                target_user = user_element;
                break;
            }
        }

        char *carro1 = cJSON_GetObjectItemCaseSensitive(target_user, "carro1")->valuestring;
        char *carro2 = cJSON_GetObjectItemCaseSensitive(target_user, "carro2")->valuestring;
        char *carro3 = cJSON_GetObjectItemCaseSensitive(target_user, "carro3")->valuestring;

        printf("\n== Informacao de carros ==\n\n");
        printf("1. %s\n2. %s\n3. %s\n4. Sair\n", carro1, carro2, carro3);
        printf("Escolha: ");
        scanf("%d", &escolha);
        while ((ch = getchar()) != '\n' && ch != EOF);
        
        switch(escolha) {
            case 1:
                gerenciarVeiculoIndividual(carro1, "carro1", "carro1_marca", "carro1_capacidade", "carro1_autonomia", "carro1_bateria", target_user, root);
                break;
            case 2:
                gerenciarVeiculoIndividual(carro2, "carro2", "carro2_marca", "carro2_capacidade", "carro2_autonomia", "carro2_bateria", target_user, root);
                break;
            case 3:
                gerenciarVeiculoIndividual(carro3, "carro3", "carro3_marca", "carro3_capacidade", "carro3_autonomia", "carro3_bateria", target_user, root);
                break;
            case 4:
                rodando_menu_carro = 0;
                cJSON_Delete(root); 
                break;
            default:
                cJSON_Delete(root);
                break;
        }
    }
    return 0;
}


void listarSessoes(Sessao sessoes[], int total) {
    if (total == 0) {
        printf("\nNenhuma sessao de recarga registrada ainda.\n");
        return;
    }

    printf("\n=======================================================\n");
    printf("                  SESSOES DE RECARGA                   \n");
    printf("=======================================================\n");
    for (int i = 0; i < total; i++) {
        printf("ID: %02d | Usuario: %-15s | Energia: %6.2f kWh\n", sessoes[i].id, sessoes[i].nome, sessoes[i].energia);
        printf("Tempo: %5.2f h | Custo: R$ %6.2f | Data: %02d/%02d/%04d %02d:%02d:%02d\n", 
               sessoes[i].tempo, sessoes[i].preco, 
               sessoes[i].dia, sessoes[i].mes, sessoes[i].ano,
               sessoes[i].hora, sessoes[i].min, sessoes[i].seg);
        printf("-------------------------------------------------------\n");
    }
}

// Busca uma sessao especifica usando seu id. Se as sessões estiverem ordenadas em qualquer forma menos a de id, tem uma chance que a Busca Binária não funcione
// por isso, foi adicionado uma Busca Linear, também.

void buscarSessao(Sessao sessoes[], int total) {

    if (total == 0) {
        printf("\nNao ha sessoes registradas para buscar.\n");
        return;
    }

    int idBuscado;
    
    printf("\nDigite o ID da sessao: ");
    scanf("%d", &idBuscado);
    while (getchar() != '\n'); 

    // Busca Binária
    int inicio = 0;
    int fim = total - 1;
    int encontrado = -1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;

        if (sessoes[meio].id == idBuscado) {
            encontrado = meio;
            break; 
        }
        else if (sessoes[meio].id < idBuscado) {
            inicio = meio + 1;
        }
        else {
            fim = meio - 1;
        }
    }

    // Busca Linear (para casos além da ordem por ID)
    if (encontrado == -1) {
        for (int i = 0; i < total; i++) {
            if (sessoes[i].id == idBuscado) {
                encontrado = i;
                break;
            }
        }
    }

    if (encontrado != -1) {
        printf("\n========= SESSAO ENCONTRADA =========\n");
        printf("ID da Sessao: %d\n", sessoes[encontrado].id);
        printf("Usuario:      %s\n", sessoes[encontrado].nome);
        printf("Energia:      %.2f kWh\n", sessoes[encontrado].energia);
        printf("Tempo:        %.2f horas\n", sessoes[encontrado].tempo);
        printf("Custo:        R$ %.2f\n", sessoes[encontrado].preco);
        printf("Horario:      %02d/%02d/%04d as %02d:%02d:%02d\n", 
               sessoes[encontrado].dia, sessoes[encontrado].mes, sessoes[encontrado].ano,
               sessoes[encontrado].hora, sessoes[encontrado].min, sessoes[encontrado].seg);
        printf("=====================================\n");
    } else {
        printf("\nErro: Sessao com o ID %d nao encontrada.\n", idBuscado);
    }
}

// Ordena as sessões de formas diferentes. Para isso, faz uso de Bubble Sort.
void ordenarSessoes(Sessao sessoes[], int total) {
    if (total <= 1) {
        printf("\nQuantidade de sessoes insuficiente para ordenacao.\n");
        return;
    }

    int opcao, ch;
    printf("\nEscolha o criterio de ordenacao:\n");
    printf("1 - Por ID\n2 - Por Energia Consumida\n3 - Por Custo da Sessao\n4 - Por Tempo de Recarga\n");
    scanf("%d", &opcao);
    while ((ch = getchar()) != '\n' && ch != EOF);

    // Bubble Sort
    for (int i = 0; i < total - 1; i++) {
        for (int j = 0; j < total - i - 1; j++) {
            int trocar = 0;

            switch (opcao) {
                case 1:
                    if (sessoes[j].id > sessoes[j + 1].id) trocar = 1;
                    break;
                case 2:
                    if (sessoes[j].energia > sessoes[j + 1].energia) trocar = 1;
                    break;
                case 3:
                    if (sessoes[j].preco > sessoes[j + 1].preco) trocar = 1;
                    break;
                case 4:
                    if (sessoes[j].tempo > sessoes[j + 1].tempo) trocar = 1;
                    break;
                default:
                    printf("\nOpcao de ordenacao invalida.\n");
                    return;
            }

            if (trocar) {
                Sessao temp = sessoes[j];
                sessoes[j] = sessoes[j + 1];
                sessoes[j + 1] = temp;
            }
        }
    }
    printf("\nSessoes ordenadas com sucesso!\n");
}

// Isso exibi as estatisticas totais de todas as sessões dessa conta.
void exibirEstatisticas(Sessao sessoes[], int total) {
    if (total == 0) {
        printf("\nNao ha dados suficientes para gerar estatisticas.\n");
        return;
    }

    float energiaTotal = 0;
    float faturamentoTotal = 0;
    float maiorConsumo = sessoes[0].energia;
    float menorConsumo = sessoes[0].energia;

    for (int i = 0; i < total; i++) {
        energiaTotal += sessoes[i].energia;
        faturamentoTotal += sessoes[i].preco;

        if (sessoes[i].energia > maiorConsumo) {
            maiorConsumo = sessoes[i].energia;
        }
        if (sessoes[i].energia < menorConsumo) {
            menorConsumo = sessoes[i].energia;
        }
    }

    float ticketMedio = faturamentoTotal / total;

    printf("\n========= ESTATISTICAS ==========\n\n");
    printf("Sessoes realizadas:  %d\n", total);
    printf("Energia fornecida:   %.2f kWh\n", energiaTotal);
    printf("Faturamento:         R$ %.2f\n", faturamentoTotal);
    printf("Ticket medio:        R$ %.2f\n\n", ticketMedio);
    printf("Maior consumo:       %.2f kWh\n", maiorConsumo);
    printf("Menor consumo:       %.2f kWh\n", menorConsumo);
    printf("=================================\n");
}

// Carrega as sessões de execuções anteriores do programa. Caso nunca foi executado antes, faz nada.
void carregarSessoes(char *name, Sessao sessoes[], int *total) {
    char buf[32768]; 
    *total = 0;      

    FILE *f = fopen(FILE_NAME, "r");
    if (!f) return; 
    
    int bytes = fread(buf, 1, sizeof(buf) - 1, f);
    buf[bytes] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(buf);
    if (!root) return;

    cJSON *target_user = NULL;
    cJSON *user_element = NULL;
    
    // Procura o usuário correto no JSON
    cJSON_ArrayForEach(user_element, root) {
        cJSON *user_obj = cJSON_GetObjectItemCaseSensitive(user_element, "username");
        if (user_obj && user_obj->valuestring && strcmp(user_obj->valuestring, name) == 0) {
            target_user = user_element;
            break;
        }
    }

    if (target_user != NULL) {
        cJSON *json_historico = cJSON_GetObjectItemCaseSensitive(target_user, "historico_sessoes");
        
        if (json_historico != NULL && cJSON_IsArray(json_historico)) {
            cJSON *sessao_item = NULL;
            
            // Percorre cada objeto dentro do array "historico_sessoes" do JSON
            cJSON_ArrayForEach(sessao_item, json_historico) {
                if (*total >= 100) break; // Proteção para não estourar o limite de 100 do seu array

                int indice = *total;
                
                // Mapeia os dados do JSON de volta para a Struct em RAM
                sessoes[indice].id = cJSON_GetObjectItemCaseSensitive(sessao_item, "id")->valueint;
                strcpy(sessoes[indice].nome, cJSON_GetObjectItemCaseSensitive(sessao_item, "usuario")->valuestring);
                sessoes[indice].energia = (float)cJSON_GetObjectItemCaseSensitive(sessao_item, "energia_kwh")->valuedouble;
                sessoes[indice].tempo = (float)cJSON_GetObjectItemCaseSensitive(sessao_item, "tempo_horas")->valuedouble;
                sessoes[indice].preco = (float)cJSON_GetObjectItemCaseSensitive(sessao_item, "preco_reais")->valuedouble;
                
                sessoes[indice].dia = cJSON_GetObjectItemCaseSensitive(sessao_item, "dia")->valueint;
                sessoes[indice].mes = cJSON_GetObjectItemCaseSensitive(sessao_item, "mes")->valueint;
                sessoes[indice].ano = cJSON_GetObjectItemCaseSensitive(sessao_item, "ano")->valueint;
                sessoes[indice].hora = cJSON_GetObjectItemCaseSensitive(sessao_item, "hora")->valueint;
                sessoes[indice].min = cJSON_GetObjectItemCaseSensitive(sessao_item, "minuto")->valueint;
                sessoes[indice].seg = cJSON_GetObjectItemCaseSensitive(sessao_item, "segundo")->valueint;

                (*total)++; // Avança o índice do vetor
            }
        }
    }

    cJSON_Delete(root); // Libera a memória do JSON
}

// Função principal que chama as outras.
int main(){

    int processoCarregador = 0;
    int opcao1;
    int opcao2;
    char senha[100];
    char nome[100] = "???";
    int ch;
    int registro = 1;
    Sessao historico_sessoes[100];
    int total_sessoes = 0;
    float tempo_simulado_total = 0.0;
    float bateria_estacao_kwh = 70.0;

    while (registro == 1){
        printf("=== BEM VINDO ===\n1. Cadastrar\n2. Login\nEscolha: ");
        scanf("%d", &opcao1);
        while ((ch = getchar()) != '\n' && ch != EOF);

        switch(opcao1){
            case 1:
            printf("Escreva seu nome: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';

            printf("Escreva sua senha: ");
            fgets(senha, sizeof(senha), stdin);
            senha[strcspn(senha, "\n")] = '\0';


            sign_up(nome, senha);

            strcpy(nome, "???");
            strcpy(senha, "???");

            break;
            
            case 2:
            printf("Escreva seu nome: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';

            printf("Escreva sua senha: ");
            fgets(senha, sizeof(senha), stdin);
            senha[strcspn(senha, "\n")] = '\0';

            if (log_in(nome, senha)) {
                printf("\nLogin feito com sucesso!\n\n");
                carregarSessoes(nome, historico_sessoes, &total_sessoes);
                registro = 0;
                processoCarregador = 1;
                break;

            } 
            else {
                printf("\nLogin falhado! Credenciais incorretas.\n");
                break;
            }


            break;
        }
    }

    while (processoCarregador == 1){ 

        interface(nome, tempo_simulado_total, bateria_estacao_kwh);

        printf("1 - Adicionar Informacao de Pagamento\n2 - Registro de Carros\n3 - Recarga\n4 - Sessoes\n5 - Finalizacao de Programa\nEscolha: ");
        scanf("%d", &opcao2);
        while ((ch = getchar()) != '\n' && ch != EOF);

        switch(opcao2){
        case 1: 

            pagamento(nome, senha);
            printf("Sua informacao agora esta registrada!\n");      
            break;
        case 2:

            carro(nome);
            break;

        case 3:

            recharge(nome, historico_sessoes, &total_sessoes, &tempo_simulado_total, &bateria_estacao_kwh); 
            break;

        case 4:
            ;
            int opcao3;
            int sessoes = 1;

            while (sessoes == 1){

                printf("\n1 - Listar sessoes\n2 - Buscar sessao\n3 - Ordenar sessoes\n4 - Estatisticas\n5 - Sair\nEscolha: ");
                scanf("%d", &opcao3);
                while ((ch = getchar()) != '\n' && ch != EOF);
                
                switch(opcao3){

                    case 1:

                        listarSessoes(historico_sessoes, total_sessoes);
                        break; 

                    case 2:

                        buscarSessao(historico_sessoes, total_sessoes);
                        break;
                    
                    case 3:

                        ordenarSessoes(historico_sessoes, total_sessoes);
                        break;

                    case 4:

                        exibirEstatisticas(historico_sessoes, total_sessoes);
                        break;

                    case 5:

                        sessoes = 0;
                        break;

                    default:

                        break;
                }
            }
            
            
            break;

        case 5: { 
            // Além de sair do programa, salva todas sessões feitas e deleta a variavel de "recarga" do json.
            // O motivo por traz dessa variavel existir é que quando carrega seu carro, não normalmente carregaria denovo na mesma sessão, então para ser realista, foi adicionada essa opção. 
            char buf[4096];

            FILE *f = fopen(FILE_NAME, "r");
            if (f != NULL) {
                int bytes = fread(buf, 1, sizeof(buf) - 1, f);
                buf[bytes] = '\0';
                fclose(f);

                cJSON *root = cJSON_Parse(buf);
                cJSON *target_user = NULL;
                cJSON *user_element = NULL;
                
                cJSON_ArrayForEach(user_element, root) {
                    cJSON *user_obj = cJSON_GetObjectItemCaseSensitive(user_element, "username");
                    
                    if (user_obj && user_obj->valuestring && strcmp(user_obj->valuestring, nome) == 0) {
                        target_user = user_element;
                        break;
                    }
                }

                if (target_user != NULL) {
                    cJSON_DeleteItemFromObject(target_user, "carro1_recarga");
                    cJSON_DeleteItemFromObject(target_user, "carro2_recarga");
                    cJSON_DeleteItemFromObject(target_user, "carro3_recarga");
                    cJSON_DeleteItemFromObject(target_user, "historico_sessoes");
                    
                    cJSON *json_historico = cJSON_GetObjectItemCaseSensitive(target_user, "historico_sessoes");
                    if (json_historico == NULL) {
                        json_historico = cJSON_CreateArray();
                        cJSON_AddItemToObject(target_user, "historico_sessoes", json_historico);
                    }

                    

                    for (int i = 0; i < total_sessoes; i++) {
                        cJSON *sessao_obj = cJSON_CreateObject();
                        
                        cJSON_AddNumberToObject(sessao_obj, "id", historico_sessoes[i].id);
                        cJSON_AddStringToObject(sessao_obj, "usuario", historico_sessoes[i].nome);
                        cJSON_AddNumberToObject(sessao_obj, "energia_kwh", historico_sessoes[i].energia);
                        cJSON_AddNumberToObject(sessao_obj, "tempo_horas", historico_sessoes[i].tempo);
                        cJSON_AddNumberToObject(sessao_obj, "preco_reais", historico_sessoes[i].preco);
                        
                        cJSON_AddNumberToObject(sessao_obj, "dia", historico_sessoes[i].dia);
                        cJSON_AddNumberToObject(sessao_obj, "mes", historico_sessoes[i].mes);
                        cJSON_AddNumberToObject(sessao_obj, "ano", historico_sessoes[i].ano);
                        cJSON_AddNumberToObject(sessao_obj, "hora", historico_sessoes[i].hora);
                        cJSON_AddNumberToObject(sessao_obj, "minuto", historico_sessoes[i].min);
                        cJSON_AddNumberToObject(sessao_obj, "segundo", historico_sessoes[i].seg);

                        cJSON_AddItemToArray(json_historico, sessao_obj);
                    }
                }

                save_json(root); 
            }


            printf("Obrigado por visitar o carregador GoodWe!");
            processoCarregador = 0;
            break;

        }
     }        
        
     
    }
    return 0;
}  