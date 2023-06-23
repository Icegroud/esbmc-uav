for model_function in model_functions_set:
K = 10
ret_BFCP = 1
ret_ICP = 0

def BFCP():
    global model_function
    k = 1
    while k <= K:
        cmd = "esbmc --no-unwinding-assertions --overflow-check --memory-leak-check --nan-check --deadlock-check --lock-order-check --atomicity-check --error-label label --base-case --forward-condition --unwind 1 --function model_function"
        subprocess.run(cmd, shell=True)
        k += 1
    ret_BFCP = 0
    while(ret_ICP == 0):
        continue
    if ret_ICP == 1 :
        return True
    else:
        print("ICP ERROR!!")
        sys.exit()

def ICP():
    global model_function
    cmd = "esbmc --no-unwinding-assertions --overflow-check --memory-leak-check --nan-check --deadlock-check --lock-order-check --atomicity-check --error-label label --inductive-step --function model_function --unwind " + K
    subprocess.run(cmd, shell=True)
    ret_ICP = 1

BFCP_thread = threading.Thread(target=BFCP, name="BFCP")
ICP_thread = threading.Thread(target=ICP, name="ICP")
BFCP_thread.start()
ICP_thread.start()

BFCP_thread.join()
ICP_thread.join()
