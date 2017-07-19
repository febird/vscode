package main
import(
	"fmt"
	"stringutil"
	"crypto/md5"
	)

func main() {
	var str="Mars Duan, Hello Go!"
	fmt.Println(str)
	fmt.Println(stringutil.Reverse(str))

	var sum []byte
	sum =  md5.New().Sum([]byte(str))
	fmt.Printf("%x\n",sum)
	
	var ch=""
	_,err := fmt.Scanf("%s",&ch)
	if nil ==  err {
	}
}

